#!/usr/bin/env python3

import os
import sys
import urllib.parse
import traceback


def get_form_data():
    if os.environ.get("REQUEST_METHOD", "").upper() == "POST":
        length = int(os.environ.get("CONTENT_LENGTH", 0))
        post_data = sys.stdin.read(length)
        return urllib.parse.parse_qs(post_data)
    return {}

def main():
    try:
        form = get_form_data()

        name = form.get("name", ["Mundo"])[0]

        print(f"""
        <html>
            <head>
                <title>Script CGI em Python</title>
            </head>
            <body>
                <h1>Olá, {name}!</h1>
                <form method="post" action="index.py">
                    Nome: <input type="text" name="name">
                    <input type="submit" value="Enviar">
                </form>
            </body>
        </html>
        """)
    except Exception as e:
        print_html_header()
        print("<h1>Erro Interno</h1>")
        print("<pre>")
        traceback.print_exc()
        print("</pre>")

if __name__ == "__main__":
    main()
