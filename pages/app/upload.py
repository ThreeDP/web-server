#!/usr/bin/env python3
import os
import cgi

# Obtém o diretório de upload da variável de ambiente SAVE_INTO
UPLOAD_DIR = os.path.join(os.getcwd(), os.getenv('SAVE_INTO'))

def save_uploaded_file(uploaded_file):
    if not uploaded_file.filename:
        return "Nenhum arquivo selecionado."

    # Certifica-se de que o diretório de upload existe
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    file_path = os.path.join(UPLOAD_DIR, os.path.basename(uploaded_file.filename))
    with open(file_path, 'wb') as f:
        f.write(uploaded_file.file.read())

    return f"Arquivo '{uploaded_file.filename}' salvo com sucesso."

def main():
    form = cgi.FieldStorage()

    message = ""
    if "file" in form:
        uploaded_file = form["file"]
        message = save_uploaded_file(uploaded_file)

    print("Content-Type: text/html\r\n\r\n")
    html = f"""
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
            <h2 class="text-center mt-4">Imagens Enviadas</h2>
            <div class="row">
                <!-- Aqui você pode adicionar o código para listar as imagens enviadas -->
            </div>
        </div>
    </body>
    </html>
    """
    print(html)

if __name__ == "__main__":
    main()
