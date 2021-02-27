build:
	docker build -t hacksore/hyundi-kia-stamp .
test:
	docker run hacksore/hyundi-kia-stamp:latest "99cfff84-f4e2-4be8-a5ed-e5b755eb6581:1614438506419"