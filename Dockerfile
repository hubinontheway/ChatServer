# 1. 构建阶段
FROM ubuntu:22.04 AS builder

# 安装所有构建依赖
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake ninja-build \
    libodb-dev libodb-boost-dev libodb-sqlite-dev libodb-mysql-dev \
    libcurl4-openssl-dev libmysqlclient-dev\
    nlohmann-json3-dev

# 设置工作目录
WORKDIR /app
COPY . .

# 构建项目
RUN cmake -S . -B cmake-build -G Ninja && \
    cmake --build cmake-build --target ChatServer

# 2. 运行阶段
FROM ubuntu:22.04

# 安装所有运行依赖
RUN apt-get update && \
    apt-get install -y \
    libstdc++6 \
    libsqlite3-0 \
    libmysqlclient21 \
    libodb-2.4 libodb-sqlite-2.4 libodb-mysql-2.4 \
    libcurl4

# 设置工作目录
WORKDIR /app

# 拷贝可执行文件和资源
COPY --from=builder /app/cmake-build/ChatServer .
COPY profile ./profile

# 启动服务
CMD ["./ChatServer"]