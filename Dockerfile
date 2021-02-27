# FROM openjdk:12
FROM maven

COPY . /workspace

WORKDIR /workspace

RUN mvn clean package

ENV LD_LIBRARY_PATH=/workspace/lib/x86_64
ENTRYPOINT ["java", "-Djava.library.path=/workspace/lib/x86_64", "-cp", "/workspace/target/main-1.0.jar", "com.hyundai.stamps.Main"]