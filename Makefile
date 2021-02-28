build:
	docker build --progress=plain -t hacksore/hks:local .
test:
	docker run hacksore/hks:local "single" "99cfff84-f4e2-4be8-a5ed-e5b755eb6581:1614438506420"
list:
	docker run hacksore/hks:local "list" "99cfff84-f4e2-4be8-a5ed-e5b755eb6581"