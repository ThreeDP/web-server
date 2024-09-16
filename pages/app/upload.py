#!/usr/bin/env python3
import os
import cgi
import html

# Obtém o diretório de upload da variável de ambiente SAVE_INTO
UPLOAD_DIR = os.getenv('SAVE_INTO', '/tmp/uploads')

def save_uploaded_file(uploaded_file):
    if not uploaded_file.filename:
        return "Nenhum arquivo selecionado."

    # Certifica-se de que o diretório de upload existe
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    # Protege contra arquivos com nomes que podem sobrescrever arquivos críticos
    file_name = os.path.basename(uploaded_file.filename)
    file_path = os.path.join(UPLOAD_DIR, file_name)
    
    # Garantir um nome de arquivo único para evitar sobrescrição
    base, ext = os.path.splitext(file_name)
    counter = 1
    while os.path.exists(file_path):
        file_path = os.path.join(UPLOAD_DIR, f"{base}_{counter}{ext}")
        counter += 1

    with open(file_path, 'wb') as f:
        f.write(uploaded_file.file.read())

    return f"Arquivo '{file_name}' salvo com sucesso."

def list_uploaded_files():
    files = os.listdir(UPLOAD_DIR)
    if not files:
        return "<p>Nenhum arquivo enviado ainda.</p>"
    
    file_links = ""
    for file_name in files:
        file_path = os.path.join(UPLOAD_DIR, file_name)
        if os.path.isfile(file_path):
            file_links += f'<li><a href="{file_path}" target="_blank">{html.escape(file_name)}</a></li>'
    
    return f"<ul>{file_links}</ul>"

def main():
    form = cgi.FieldStorage()

    message = ""
    if "file" in form:
        uploaded_file = form["file"]
        message = save_uploaded_file(uploaded_file)

    # Imprimir o cabeçalho HTTP
    print("Content-Type: text/html\r\n\r\n")
    
    # Gerar a página HTML
    html_content = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Upload de Arquivo</title>
        <!-- Bootstrap CSS -->
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css" rel="stylesheet">
    </head>
    <body>
        <div class="container mt-5">
            <h1 class="text-center">Upload de Arquivo</h1>
            <form enctype="multipart/form-data" action="/upload.py" method="POST">
                <div class="mb-3">
                    <label for="file" class="form-label">Escolha um arquivo para enviar</label>
                    <input type="file" class="form-control" id="file" name="file" required>
                </div>
                <button type="submit" class="btn btn-primary">Enviar Arquivo</button>
            </form>
            <p class="text-success mt-3">{message}</p>
            <h2 class="text-center mt-4">Arquivos Enviados</h2>
            {list_uploaded_files()}
        </div>
    </body>
    </html>
    """
    
    print(html_content)

if __name__ == "__main__":
    main()
