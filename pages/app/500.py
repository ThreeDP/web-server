#!/usr/bin/env python3
import urllib.request
import json

# Substitua "YOUR_API_KEY" pela sua chave de API
API_URL = "https://api.thecatapi.com/v1/images/search?limit=1"
API_KEY = "live_FeSZjr463jttvoElOQds4u6awJ4yD1mdhETYA0bmAGwneFINYiVcBwLiELkySpFy"

# Função para obter uma imagem aleatória de gato
def get_random_cat_image():
    req = urllib.request.Request(API_URL)
    req.add_header("x-api-key", API_KEY)
    with urllib.request.urlopen(req) as response:
        data = response.read()
        images = json.loads(data.decode("utf-8"))
        return images[0]['url']
    return None

def render_500_page(cat_image_url):
    html = "HTTP/1.1 500 Internal Server Error\r\n"
    html += "Content-Type: text/html\r\n\r\n"
    html += f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>500 - Erro Interno do Servidor</title>
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css" rel="stylesheet">
        <style>
            body {{
                background-color: #f8f9fa;
                color: #343a40;
            }}
            .error-container {{
                margin-top: 100px;
            }}
            .cat-image {{
                max-height: 400px;
                object-fit: cover;
                margin-bottom: 20px;
                border-radius: 10px;
            }}
            .btn-home {{
                margin-top: 20px;
                padding: 10px 20px;
            }}
        </style>
    </head>
    <body>
        <div class="container error-container text-center">
            <h1 class="display-4 text-danger">500 - Erro Interno do Servidor</h1>
            <p class="lead">Ocorreu um erro inesperado no servidor.</p>
            <p>Enquanto tentamos resolver o problema, aqui está uma foto de um gato para alegrar seu dia!</p>
            <img src="{cat_image_url}" class="img-fluid cat-image" alt="Imagem de Gato Aleatório">
            <div>
                <a href="/" class="btn btn-primary btn-home">Voltar para a Página Inicial</a>
            </div>
        </div>
        <footer class="text-center mt-5">
            <p class="text-muted">&copy; 2024 My Simple CGI Website</p>
        </footer>
    </body>
    </html>
    """
    print(html)

def main():
    # Obter uma imagem aleatória de gato
    cat_image_url = get_random_cat_image()

    if not cat_image_url:
        # Imagem fallback se a API falhar
        cat_image_url = "https://cdn2.thecatapi.com/images/MTY3ODIyMQ.jpg"

    # Renderiza a página 500 com a imagem do gato
    render_500_page(cat_image_url)

if __name__ == "__main__":
    main()