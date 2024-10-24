# Use an appropriate base image, e.g., Ubuntu
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV SDK_DIR=/opt/vex-sdk
ENV PATH=$SDK_DIR/bin:$PATH

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    libssl-dev \
    libffi-dev \
    && rm -rf /var/lib/apt/lists/*

# Clone Vex-SDK repository
RUN git clone https://github.com/RanchoDVT/Vex-SDK.git $SDK_DIR

# Change to the SDK folder and build the SDK
WORKDIR $SDK_DIR
RUN cd v5 && make install

# Copy entrypoint script
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Define entrypoint
ENTRYPOINT ["/entrypoint.sh"]
