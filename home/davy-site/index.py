#!/usr/bin/env python3

import os
import sys
import traceback


def main():
    try:
        method = str(os.environ.get("REQUEST_METHOD", "").upper())
        length = int(os.environ.get("CONTENT_LENGTH", 0))
        post_data = sys.stdin.read(length)
        name = str(os.environ.get("NAME", ""))
        response_text = str(os.environ.get("DESCRIPTION", ""))
        print(f"""
        <html>
            <head>
                <title>Script CGI em Python</title>
            </head>
            <body>
                <h1>Resposta do Servidor</h1>
                <p>O valor enviado foi: {name}</p>
                <p>Method: {method}</p>
                <pre>{response_text}</pre>
                <form method="post" action="index.py">
                    Nome: <input type="text" name="name">
                    <input type="submit" value="Enviar">
                </form>
            </body>
        </html>
        """)
    except Exception as e:
        print("Content-Type: text/html\n")
        print("<h1>Erro Interno</h1>")
        print("<pre>")
        traceback.print_exc()
        print("</pre>")

if __name__ == "__main__":
    main()
