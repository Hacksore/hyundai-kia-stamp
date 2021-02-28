FROM maven

COPY . /workspace
WORKDIR /workspace

RUN mvn clean package
ENV LD_LIBRARY_PATH=/workspace/lib/x86_64

ENTRYPOINT java -Djava.library.path=$LD_LIBRARY_PATH -jar /workspace/target/main-1.0.jar $0 $1