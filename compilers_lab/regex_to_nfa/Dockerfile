FROM alpine:latest
RUN apk add --no-cache gcc musl-dev make cmake
WORKDIR /app
COPY . .
CMD ["sh", "-c", "cmake . && make && ./validator"]