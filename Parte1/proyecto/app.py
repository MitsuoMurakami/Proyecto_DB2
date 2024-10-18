import pandas as pd
from flask import Flask, render_template, request

app = Flask(__name__)


input_history = []

@app.route("/", methods=["GET", "POST"])
def index():
    global input_history
    
    csv_data = []
    selected_method = "method1"  
    
    if request.method == "POST":
        input_text = request.form.get("input_text")
        selected_method = request.form.get("method")  

        if input_text:
            input_history.append(input_text)

            # metodo1
            if selected_method == "method1" and input_text.lower() == "tabla":
                data = pd.read_csv("data.csv", delimiter=';')
                csv_data = data.to_dict(orient='records')
            #cuando este conectado con C++:
            #if selected_method == "method1":
                #metodo(input_text)
                #data = pd.read_csv("resultado.csv", delimiter=';')
                #csv_data = data.to_dict(orient='records')
            #metodo2
            elif selected_method == "method2" and input_text.lower() == "tabla":
                data = pd.read_csv("cars2.csv", delimiter=';')
                csv_data = data.to_dict(orient='records')
            #cuando este conectado con C++:
            #if selected_method == "method2":
                #metodo(input_text)
                #data = pd.read_csv("resultado.csv", delimiter=';')
                #csv_data = data.to_dict(orient='records')
            #metodo3
            elif selected_method == "method3" and input_text.lower() == "diego":
                data = pd.read_csv("cars2.csv", delimiter=';')
                csv_data = data.to_dict(orient='records')
            #cuando este conectado con C++:
            #if selected_method == "method3":
                #metodo(input_text)
                #data = pd.read_csv("resultado.csv", delimiter=';')
                #csv_data = data.to_dict(orient='records')
    return render_template("index.html", csv_data=csv_data, selected_method=selected_method)

if __name__ == "__main__":
    app.run(debug=True)
