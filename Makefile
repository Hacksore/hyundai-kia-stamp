build:
	docker build --platform=linux/arm/v8 --progress=plain -t hacksore/hks:local .
test:
	docker run hacksore/hks:local "hyundai" "single" "99cfff84-f4e2-4be8-a5ed-e5b755eb6581:1614438506420"
testKia:
	docker run hacksore/hks:local "kia" "single" "693a33fa-c117-43f2-ae3b-61a02d24f417:1614438506420"
list:
	docker run hacksore/hks:local "hyundai" "list" "99cfff84-f4e2-4be8-a5ed-e5b755eb6581"
listKia:
	docker run hacksore/hks:local "kia" "list" "693a33fa-c117-43f2-ae3b-61a02d24f417"
