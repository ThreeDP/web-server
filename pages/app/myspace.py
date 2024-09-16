import os

IMAGE_DIR = "./images"  # Diretório onde estão as imagens

# Função para pegar todos os arquivos no diretório de imagens
def get_image_files(directory):
    try:
        return [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    except FileNotFoundError:
        return []

# Pega todos os arquivos do diretório
image_files = get_image_files(IMAGE_DIR)

print("HTTP/1.1 200 OK\r\n")
print("Content-Type: text/html\r\n\r\n")
html = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple CGI Page</title>
    <!-- Bootstrap CSS -->
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
</head>
<body>
    <!-- Navigation Bar -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
        <div class="container-fluid">
            <a class="navbar-brand" href="#">My Website</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse" id="navbarNav">
                <ul class="navbar-nav">
                    <li class="nav-item">
                        <a class="nav-link active" aria-current="page" href="index.py">Home</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="calculator.py">Calculator</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="upload.py">Upload</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="myspace.py">My Space</a>
                    </li>
                </ul>
            </div>
        </div>
    </nav>

    <div class="container mt-5">
        <h1 class="text-center mb-4">My Space</h1>
        <div class="row">"""


for img_file in image_files:
    img_path = f"/images/{img_file}"
    html += f"""
            <div class="col-md-4 mb-4">
                <div class="card">
                    <img src="{img_path}" class="card-img-top" alt="{img_file}">
                    <div class="card-body">
                        <p class="card-text text-center">{img_file}</p>
                    </div>
                </div>
            </div>"""

html += """
            </div>
    </div>

    <!-- Footer -->
    <footer class="bg-dark text-white text-center mt-5 p-3">
        <p>© 2024 My Simple CGI Website</p>
    </footer>

    <!-- Bootstrap JS and dependencies -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz" crossorigin="anonymous"></script>
</body>
</html>
"""

print(html)
