from flask import Flask, render_template, request

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def home():
    resultado = None

    if request.method == 'POST':
        numero = int(request.form['numero'])
        if numero %2 == 0:
            resultado = 'Par'
        else:
            resultado = 'Impar'

    return render_template('index.html', resultado=resultado)
   

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)