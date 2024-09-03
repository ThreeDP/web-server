#!/usr/bin/env python3

import os
import sys
import urllib.parse
import json
import traceback

def get_form_data():
    """Obtém os dados do formulário enviado via POST."""
    if os.environ.get("REQUEST_METHOD", "").upper() == "POST":
        length = int(os.environ.get("CONTENT_LENGTH", 0))
        post_data = sys.stdin.read(length)
        return json.loads(post_data)
    return {}

def generate_query_string(params):
    """Gera uma string de consulta a partir de um dicionário de parâmetros."""
    return urllib.parse.urlencode(params)

def main():
    try:
        # Se o método for POST, processa os dados do formulário
        if os.environ.get("REQUEST_METHOD", "").upper() == "POST":
            form_data = get_form_data()
            # Gera a string de consulta com base nos dados do formulário
            query_string = generate_query_string(form_data)
            # Redireciona para a página com os parâmetros de consulta
            redirect_url = f'index.py?{query_string}'
            
            print("Content-Type: text/html\n")
            print(f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Formulário de Query Params</title>
                <meta http-equiv="refresh" content="0;url={redirect_url}">
            </head>
            <body>
                <h1>Redirecionando...</h1>
                <p>Você será redirecionado em breve. Se o redirecionamento não ocorrer, <a href="{redirect_url}">clique aqui</a>.</p>
            </body>
            </html>
            """)
        else:
            # Exibe o formulário se não for uma requisição POST
            print(f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Formulário de Query Params</title>
            </head>
            <body>
                <h1>Preencha os Parâmetros de Consulta</h1>
                <form method="post" action="index.py">
                    <label for="name">Nome:</label>
                    <input type="text" id="name" name="NAME" required>
                    <br>
                    <label for="age">Idade:</label>
                    <input type="text" id="age" name="AGE">
                    <br>
                    <label for="city">Cidade:</label>
                    <input type="text" id="city" name="CITY">
                    <br>
                    <input type="submit" value="Enviar">
                </form>
            </body>
            </html>
            """)
    except Exception as e:
        print("<h1>Erro Interno</h1>")
        print("<pre>")
        traceback.print_exc()
        print("</pre>")

if __name__ == "__main__":
    main()
