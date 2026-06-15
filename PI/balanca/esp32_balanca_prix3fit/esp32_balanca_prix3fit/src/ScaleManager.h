// =============================================================================
// ScaleManager.h
//
// Responsabilidade: toda comunicação com a balança Prix 3 Fit via RS232.
// Implementa máquina de estados para parser de stream serial contínuo.
//
// Protocolo Toledo Continuous Output (Prix 3 Fit):
//   STX [status1] [status2] [sign] [XXXXXX.XXX] [unidade] ETX CR LF
//   Exemplo real:  \x02 S S + 0001.350 kg \x03 \r \n
//   Comprimento do pacote: variável (aprox. 20-24 bytes)
//
// Máquina de estados:
//   IDLE     → aguardando STX (0x02)
//   READING  → acumulando bytes no buffer
//   PARSING  → pacote completo (ETX detectado), faz parse
//   READY    → peso válido disponível para consumo
//   ERROR    → pacote inválido, flush e volta a IDLE
//
// Design: não-bloqueante, usa millis() para todos os timeouts.
// =============================================================================
#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include "config.h"

// ---------------------------------------------------------------------------
// Dados retornados ao chamador após parse bem-sucedido
// ---------------------------------------------------------------------------
struct ScaleReading {
    float   weight_kg;       // Peso em kg (sempre positivo)
    bool    is_stable;       // true = "S" no status, false = instável/em movimento
    bool    is_valid;        // Leitura bem formada e checksumável
    uint32_t timestamp_ms;  // millis() no momento da recepção
    char    raw[SCALE_BUFFER_SIZE]; // Pacote bruto para debug
};

// ---------------------------------------------------------------------------
// Estados internos da máquina de estados do parser serial
// ---------------------------------------------------------------------------
enum class ScaleState : uint8_t {
    IDLE,       // Aguardando início de pacote (STX)
    READING,    // Dentro de um pacote, acumulando bytes
    PARSING,    // ETX recebido, processando pacote
    READY,      // Dado pronto para ser lido externamente
    ERROR       // Pacote inválido — vai fazer flush
};

// ---------------------------------------------------------------------------
// ScaleManager — Singleton-like (instanciado uma vez em main.cpp)
// ---------------------------------------------------------------------------
class ScaleManager {
public:
    // -------------------------------------------------------------------------
    // Construtor: recebe referência para o HardwareSerial (Serial2)
    // -------------------------------------------------------------------------
    explicit ScaleManager(HardwareSerial& serial);

    // -------------------------------------------------------------------------
    // Inicializa UART2 com os pinos e baud rate configurados
    // Deve ser chamado em setup()
    // -------------------------------------------------------------------------
    void begin();

    // -------------------------------------------------------------------------
    // Processa bytes disponíveis no buffer serial — chamar a cada iteração
    // do loop(). NÃO bloqueante. Retorna true se nova leitura estiver pronta.
    // -------------------------------------------------------------------------
    bool update();

    // -------------------------------------------------------------------------
    // Retorna a leitura mais recente (chamar apenas se update() retornou true
    // ou se hasNewReading() == true). Consome a leitura (marca como lida).
    // -------------------------------------------------------------------------
    ScaleReading getReading();

    // -------------------------------------------------------------------------
    // Verifica se há nova leitura disponível sem consumi-la
    // -------------------------------------------------------------------------
    bool hasNewReading() const { return _state == ScaleState::READY; }

    // -------------------------------------------------------------------------
    // Retorna o estado atual da máquina para debug/monitoramento
    // -------------------------------------------------------------------------
    ScaleState getState() const { return _state; }

    // -------------------------------------------------------------------------
    // Estatísticas de operação (para logs e monitoramento)
    // -------------------------------------------------------------------------
    uint32_t getPacketsReceived()  const { return _stats.packets_received; }
    uint32_t getPacketsInvalid()   const { return _stats.packets_invalid; }
    uint32_t getFlushCount()       const { return _stats.flush_count; }
    uint32_t getTimeouts()         const { return _stats.timeouts; }
    float    getLastWeight()       const { return _lastReading.weight_kg; }

    // -------------------------------------------------------------------------
    // Força limpeza completa do buffer (recuperação de estado inválido)
    // -------------------------------------------------------------------------
    void flush();

private:
    HardwareSerial& _serial;

    // Estado da máquina
    ScaleState _state = ScaleState::IDLE;

    // Buffer circular para acúmulo de pacote
    char     _buf[SCALE_BUFFER_SIZE];
    uint8_t  _bufLen  = 0;

    // Controle de tempo para timeout de leitura parcial
    uint32_t _readStartMs = 0;

    // Debounce — armazena última leitura para comparar
    ScaleReading _lastReading{};
    uint32_t     _lastReadingMs = 0;

    // Contador de bytes inválidos consecutivos (trigger para flush forçado)
    uint16_t _garbageCount = 0;

    // Última leitura pronta para ser consumida pelo loop principal
    ScaleReading _pendingReading{};

    // Estatísticas internas
    struct Stats {
        uint32_t packets_received = 0;
        uint32_t packets_invalid  = 0;
        uint32_t flush_count      = 0;
        uint32_t timeouts         = 0;
    } _stats;

    // -------------------------------------------------------------------------
    // Métodos privados de parsing
    // -------------------------------------------------------------------------

    // Processa um único byte recebido — avança a máquina de estados
    void _processByte(uint8_t byte);

    // Faz parse do buffer acumulado após receber ETX
    // Retorna true se o pacote for válido e weight_kg foi preenchido
    bool _parsePacket(ScaleReading& out);

    // Verifica se nova leitura é diferente da anterior (filtragem de ruído)
    bool _isNewReading(const ScaleReading& r) const;

    // Converte string de peso "XXXXXX.XXX" para float
    // Retorna false se formato inválido
    bool _parseWeightString(const char* str, float& weight_out);

    // Checa se o byte é um caractere ASCII imprimível ou de controle esperado
    bool _isValidByte(uint8_t byte) const;

    // Verifica timeout de leitura parcial
    void _checkTimeout();
};
