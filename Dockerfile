FROM nginx

COPY nginx.conf /etc/nginx/nginx.conf
COPY home /home
COPY home/app /app
RUN chmod 777 /home
RUN chmod 777 /home/davy-site
RUN chmod 777 /home/davy-site/new

EXPOSE 8081
EXPOSE 8082
EXPOSE 8083
EXPOSE 8084
