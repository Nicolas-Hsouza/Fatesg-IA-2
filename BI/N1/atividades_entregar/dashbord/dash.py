import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv(r"C:\Users\nicol\Desktop\dashbord\financiamentoPecuaria.csv" , encoding='utf8' , sep=';')
df.head()

df= df.replace('-', 0)
df.head(100)

for col in df.columns[1:]:
    df[col] = pd.to_numeric(
        df[col].astype(str).str.replace('.', '', regex=False).str.replace(',', '.', regex=False),
        errors='coerce'
    )

df.head(10)

numeric_cols = df.select_dtypes(include=['float', 'int']).columns


df[numeric_cols] = df[numeric_cols].round(2)

df["Md_Invest"] = df.iloc[:, 1:].mean(axis=1)


df["Md_Invest"] = df["Md_Invest"].round(2)

mean_values = df.iloc[:, 1:].mean()


nova_linha = ["Média"] + mean_values.tolist()


linha_media = pd.DataFrame([nova_linha], columns=df.columns)


df = pd.concat([df, linha_media], ignore_index=True)



st.set_page_config(layout="wide")


st.title('Dashboard Financeiro - Agro Goiás - 2014-2024')
st.write('Dashboard Financeiro - Tabela')


df["Md_Invest"] = df.iloc[:, 1:].mean(axis=1)
df["Md_Invest"] = df["Md_Invest"].round(2)
st.dataframe(df.style.format({'Md_Invest': '{:.2f}'}))
st.dataframe(df.head())

df["Localidade"] = df["Localidade"].astype(str).str.strip()

localidades = df.loc[df["Localidade"] != "Média", "Localidade"].unique().tolist()

opcoes_localidade = ["Todos"] + localidades
localidade_selecionada = st.sidebar.selectbox("Selecione a Localidade:", opcoes_localidade)

colunas_anos = df.columns[1:-1]

if localidade_selecionada == "Todos":
   
    df_plot = df[df["Localidade"] != "Média"]
    
    investimentos_ano = df_plot[colunas_anos].mean()
    st.write("Média dos investimentos por ano (Todas as localidades):")
    st.bar_chart(investimentos_ano)
else:
    
    df_local = df[df["Localidade"] == localidade_selecionada]

    st.write(f"Dados filtrados para {localidade_selecionada}:")
    st.dataframe(df_local)
    if not df_local.empty:
        investimentos_local = df_local.iloc[0][colunas_anos]
        st.write(f"Evolução dos investimentos para {localidade_selecionada}:")
        st.bar_chart(investimentos_local)
    else:
        st.error("Nenhum dado encontrado para essa localidade. Confira o nome exato.")


col1, col2 = st.columns(2)

col3, col4, col5 = st.columns(3)

with col1:
    
    if localidade_selecionada == "Todos":
        
        df_plot = df[df["Localidade"] != "Média"]
        
        investimentos_ano = df_plot[colunas_anos].mean()
        st.write("### Média dos investimentos por ano (Todas as localidades):")
        st.bar_chart(investimentos_ano)
    else:
        
        df_local = df[df["Localidade"] == localidade_selecionada]
        
        st.write(f"Dados filtrados para {localidade_selecionada}:")
        st.dataframe(df_local)
        if not df_local.empty:
            investimentos_local = df_local.iloc[0][colunas_anos]
            st.write(f"Evolução dos investimentos para {localidade_selecionada}:")
            st.bar_chart(investimentos_local)
        else:
            st.error("Nenhum dado encontrado para essa localidade. Confira o nome exato.")
            
with col2:
    st.write("### Gráfico de Linhas: Evolução dos Investimentos")
    
    if localidade_selecionada == "Todos":
        investimentos_ano = df[df["Localidade"] != "Média"].loc[:, colunas_anos].mean()
    else:
        investimentos_ano = df_local.iloc[0][colunas_anos]
    st.line_chart(investimentos_ano)
    
with col3:
    st.write("### Comparação da Cidade com a Média Geral")
    
    if localidade_selecionada != "Todos" and not df_local.empty:
       
        investimentos_local = df_local.iloc[0][colunas_anos]
       
        investimentos_media = df[df["Localidade"] != "Média"].loc[:, colunas_anos].mean()
      
        df_comparacao = pd.DataFrame({
            "Ano": colunas_anos,
            localidade_selecionada: investimentos_local.values,
            "Média Geral": investimentos_media.values
        })
        
        df_comparacao.set_index("Ano", inplace=True)
        
        st.bar_chart(df_comparacao)
    else:
        st.info("Selecione uma localidade para visualizar a comparação.")
        
with col4:
   
    st.write("### Ano com Maior Investimento")
    
    if localidade_selecionada != "Todos" and not df_local.empty:
        
        investimentos_local = df_local.iloc[0][colunas_anos]
        
        ano_max = investimentos_local.idxmax()
        valor_max = investimentos_local.max()
        
        st.metric(label="Ano com Maior Investimento", value=ano_max, delta=f"Valor: {valor_max:.2f}")
    else:
        st.info("Selecione uma localidade específica para visualizar o ano com maior investimento.")