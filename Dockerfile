FROM nginx

COPY nginx.conf /etc/nginx/nginx.conf
COPY home /home
COPY home/app /app

EXPOSE 8081
EXPOSE 8082
EXPOSE 8083
EXPOSE 8084
