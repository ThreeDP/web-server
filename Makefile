all:
	docker rm -f nginx-test
	docker build -t custom-nginx .
	docker run -p 8081:8081 -p 8082:8082 -p 8083:8083 -p 8084:8084 --name nginx-test -d custom-nginx