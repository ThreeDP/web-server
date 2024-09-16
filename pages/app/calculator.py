#!/usr/bin/env python3
import os
import urllib.parse

# Obtém a QUERY_STRING do ambiente
query_string = os.environ.get('QUERY_STRING', '')

# Faz o parsing da query string
params = urllib.parse.parse_qs(query_string)

# Pega os valores de num1, num2 e operation da query string
num1 = float(params.get('num1', [0])[0])  # Pega 'num1' ou usa 0 como padrão
num2 = float(params.get('num2', [0])[0])  # Pega 'num2' ou usa 0 como padrão
operation = params.get('operation', ['add'])[0]  # Pega 'operation' ou usa 'add' como padrão

# Realiza a operação
if operation == "add":
    result = num1 + num2
elif operation == "sub":
    result = num1 - num2
elif operation == "mul":
    result = num1 * num2
elif operation == "div" and num2 != 0:
    result = num1 / num2
else:
    result = "Operação inválida ou divisão por zero"

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
        <h1 class="text-center">Calculadora Simples</h1>
        <p>Resultado: <b>{result}</b></p>   
        <form action="/calculator.py" method="get">
            <div class="row mb-3">
                <label for="num1" class="col-sm-2 col-form-label">Número 1</label>
                <div class="col-sm-10">
                    <input type="number" class="form-control" id="num1" name="num1" placeholder="Digite o primeiro número">
                </div>
            </div>
            <div class="row mb-3">
                <label for="num2" class="col-sm-2 col-form-label">Número 2</label>
                <div class="col-sm-10">
                    <input type="number" class="form-control" id="num2" name="num2" placeholder="Digite o segundo número">
                </div>
            </div>
            <div class="row mb-3">
                <label for="operation" class="col-sm-2 col-form-label">Operação</label>
                <div class="col-sm-10">
                    <select class="form-select" id="operation" name="operation">
                        <option value="add">Somar</option>
                        <option value="sub">Subtrair</option>
                        <option value="mul">Multiplicar</option>
                        <option value="div">Dividir</option>
                    </select>
                </div>
            </div>
            <button type="submit" class="btn btn-primary">Calcular</button>
        </form>
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