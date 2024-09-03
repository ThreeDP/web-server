#!/usr/bin/env python3

import os
import cgi
import cgitb
import sys

# Habilita a exibição de erros no navegador (útil para debugging)
cgitb.enable()

# Define o diretório onde as imagens serão salvas (neste caso, a pasta atual)
UPLOAD_DIR = os.path.dirname(os.path.abspath(__file__))

def save_uploaded_file(fileitem):
    """Salva o arquivo enviado no diretório de upload."""
    if fileitem.filename:
        # Extrai o nome do arquivo
        filename = os.path.basename(fileitem.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)
        
        # Escreve o arquivo no disco
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())
        
        return filepath
    return None

def main():
    # Obtém os dados do formulário
    form = cgi.FieldStorage()
    
    # Recupera o arquivo enviado
    fileitem = form['file']
    
    # Salva o arquivo no diretório especificado
    saved_file_path = save_uploaded_file(fileitem)
    
    # Responde ao navegador
    print("Content-Type: text/html\n")
    if saved_file_path:
        print(f"<h1>Upload bem-sucedido!</h1>")
        print(f"<p>O arquivo foi salvo como: {saved_file_path}</p>")
    else:
        print("<h1>Erro ao enviar o arquivo.</h1>")

if __name__ == "__main__":
    main()
