# Docker
## Step 1: Why Docker?
- Docker is the biggest runtime for creating the containers.

- Docker files are created for every microservice which are created in a monorepo.

- We should containerize all the microservices.

- Docker lets us create containers, which eventually lets us facilitate the container orchestration.

- We do not have to install the dependencies or give the access of our file system to a malicious code in the global environment, we can do this in the virtual environment with the help of the Docker.
![alt text](images/image-1.png)

- Docker/containers are important for a few reasons -
  1. Kubernetes/Container orchestration
  2. Running processes in isolated environments
![alt text](image.png)

```cmd
    node index.js
    docker run index.js
```
  3. Starting projects/auxilary services locally
     - We can start the redis, mongodb, or postgresql or any other database globally or in a docker container.
     - For every open source project to start, there are two ways to start the project, `manual installation` or `docker installation`.
     - Every good open source project has the other way of execution called the docker installation.
     - The `Docker` way would be of much less hassle.
     - We can use `actionable docker` as a tool for our daily basis task.

## Step 2: Containerization
### What are containers?
![alt text](images/image-2.png)

- **Containers** are a `way` to `package and distribute software applications` in a way that makes them `easy to deploy` and `run` consistently across `different environments`.

- They allow you to `package an application`, along with all its `dependencies` and `libraries`, into a `single unit` that can be run on any machine with a container runtime, such as Docker.

- We get a networking system and several other systems along with the file system in the docker container.

### Why containers?
1. Everyone has different Operating systems.

2. Steps to run a project can vary based on Operating System.

3. Extremely harder to keep track of dependencies as project grows

### Benefits of using containers
![alt text](images/image-3.png)
1. Let you describe your `configuration` in a single file, the Docker image which is run using the command `docker run imageCreated` is created using a configuration file.

2. Can run in isolated environments

3. Makes Local setup of Open Source projects a breeze.

4. Makes installing auxiliary services/DBs easy. 

### References
- For Reference, the following command starts mongo in all operating systems - 
```cmd
    docker run -d -p 27017:27017 mongo
```
-  Docker isn’t the only way to create containers

## Step 3:
![alt text](images/image-4.png)
- Docker is a YC backed company, started in ~2014

- They envisioned a world where containers would become mainstream and people would deploy their applications using them. That is mostly true today.

- Most projects that you open on Github will/should have docker files in them (a way to create docker containers).

- [Reference Documents](https://www.ycombinator.com/blog/solomon-hykes-docker-dotcloud-interview/)

## Step 4:
- [Docket Installation Link](https://docs.docker.com/engine/install/)
![alt text](images/image-6.png)

- Make sure you’re able to run the docker cli locally -
![alt text](images/image-5.png)

## Step 5 - Inside docker
![alt text](images/image-7.png)

- As an application/full stack developer, you need to be comfortable with the following terminologies -
1. **Docker Engine**
2. **Docker CLI** - Command line interface
3. **Docker registry**

### 1. Docker Engine
- Docker Engine is an open-source `containerization` technology that allows developers to package applications into `container`.

- Containers are standardized executable components combining application source code with the operating system (OS) libraries and dependencies required to run that code in any environment.

### 2. Docker CLI
- The command line interface lets you talk to the `docker engine` and lets you start/stop/list containers.
```cmd
    docker run -d -p 27017:27017 mongo
```

- Docker cli is not the only way to talk to a docker engine. You can hit the docker `REST` API to do the same things

### 3. Docker registry
- The `docker` registry is how Docker makes money. 
- It is similar to github, but it lets you push images rather than sourcecode.
- [Docker’s main registry](https://dockerhub.com/)
- [Mongo image on docker registry](https://hub.docker.com/_/mongo)
- When we run `npm install express` the code for the express comes to the local machine as a registry, pip registry for python, github registry.

- When we run the command `docker run mongo` in the Terminal it checks the `Docker Engine` and if it is not found in the `Docker Engine`, it gets the image from the `Docker registry` and it executes it locally.

- With npm we get files, with docker we get a small machine(container) which runs locally.
- Docker registry is a container registry, like github is a code registry.
- Port Mapping of MongoDB to 27017 using the command:
```cmd
  docker run -p 27017:27017 mongo
```

- Remove the image from the system using the command
```cmd
  docker rmi --force mongo
```

- To list down the images of the docker file:
```cmd
  docker images
```

## Step 6 - Images vs containers
### Docker Image
- A Docker image is a lightweight, standalone, executable package that includes everything needed to run a piece of software, including the code, a runtime, libraries, environment variables, and config files.

- A good mental model for an image is `Your codebase on github`.

### Docker Container
- A container is a running instance of an image. It encapsulates the application or service and its dependencies, running in an isolated environment.

- A good mental model for a container is when you run `node index.js` on your machine from some source code you got from github.

### NOTE:
- We wil have the image in our machine, irrespective of the fact that we run the container or not.

- Command to check that which containers are running.
```cmd
  docker ps
```

- We can start multiple containers of docker, it can be done using multiple containers.


## Step 7 - Port Mapping to Container
- If we run the container using the command without port mapping, it will start the container with the port no. but it is not exposed to the port no. of the machine until we do the port mapping.
```cmd
  docker run mongo
```

- Do the port mapping as follows, such that the machine's port is mapped to the container port:
```cmd
  docker run -d -p 27018:27017 mongo
```
![alt text](images/image-8.png)

- Now the windows machine's port 27018 will map to 27017.
```cmd
  docker run -d -p 27018:27017 mongo
```

- Now both the mongodb will run seperately on port 27017 and 27018.

## Step 8 - Common Docker Commands
1. docker images
2. docker ps
3. docker run
4. docker build

### 1. **docker images**
Shows you all the images that you have on your machine

### 2. **docker ps**
Shows you all the containers you are running on your machine

### 3. **docker run**
Lets you start a container
-p ⇒ let’s you create a port mapping
-d. ⇒ Let’s you run it in detatched mode

### 4. **docker build**
Lets you build an image. We will see this after we understand how to create your own `Dockerfile`

### 5. **docker push**
Lets you push your image to a registry

### 6. **Extra commands**
1. docker kill - To kill the container instance of the image this command is used.
2. docker exec - To execute the command in the docker container this command is used.
```cmd
  docker exec -it CONATINER_ID /bin/bash
```

## Step 9 - Dockerfile
### What is a Dockerfile?
- If you want to create an image from your own code, that you can push to `dockerhub`, you need to create a `Dockerfile` for your application.

- A Dockerfile is a text document that contains all the commands a user could call on the command line to create an image.

- A use case to give the container to everyone to use it to scrape the data.

- Build a NodeJS application with the following commands:
```cmd
  mkdir nodejs-project
  cd nodejs-project/
  npm init
  npm i express
```

- Create a `Dockerfile` in the root folder of the nodejs project, to containerize the application.

### How to write a dockerfile?
- A docker file has 2 parts:
  1. Base Image - If we know that the project will be a NodeJS, then we can start the project with the 
  2. Bunch of commands that you run on the base image (to install dependencies like Node.js)

### Let's write our own docker file:
- Let’s try to containerise this backend app - https://github.com/100xdevs-cohort-2/week-15-live-1
![alt text](images/image-9.png)


### Common commands
- `WORKDIR`: Sets the working directory for any RUN, CMD, ENTRYPOINT, COPY instructions that follow it.

- `RUN`: Executes any commands in a new layer on top of the current image and commits the results.
- `CMD`: Provides defaults for executing a container. There can only be one CMD instruction in a Dockerfile.

- `EXPOSE`: Informs Docker that the container listens on the specified network ports at runtime.

- `ENV`: Sets the environment variable.

- `COPY`: Allow files from the Docker host to be added to the Docker image

- We will be creating an /app directory in the container we are creating.
![alt text](images/image-10.png)
```docker
# Base Image 
FROM node:22-alpine

# Create the working directory in the container
WORKDIR /app

# Copy all the files from the file system to the container
# COPY ./index.js ./index.js
# COPY ./package.json ./package.json
# Do not copy the node_modules in the container add the nodemodules in the .dockerignore
COPY . .

# Install the dependencies freshly
RUN npm install

# Expose the port 3000 from the process
EXPOSE 3000

# Creation of the Image uses the RUN command
# Executing the container uses the CMD 
# Execute NodeJS Software node index.js
CMD ["node", "index.js"]
```

## Step 10 - Building Images
- Now that you have a dockerfile in your project, try building a docker image from it.
```cmd
  docker build -t hello_world .
```

- Now if you try to look at your images, you should notice a new image created
```cmd
  docker images
```

- Add a .dockerignore so that node_modules don’t get copied over 

## Step 11 - Running images
- Run the image via docker using the command.
```cmd
  docker run -p 3000:3000 hello-world
```
![alt text](images/image-11.png)

## Step 12 - Passing in env variables
```cmd
  docker run -p 3000:3000 -e DATABASE_URL="URL" image_name
```
- The -e argument let’s you send in environment variables to your node.js app

## Step 13 - More commands
1. docker kill - to kill a container
2. docker exec - to exectue a command inside a container
 
- Examples
1. List all contents of a container folder
```cmd
  docker exec <container_name_or_id> ls /path/to/directory
```

2. Running an Interactive Shell
```cmd
  docker exec -it <container_name_or_id> /bin/bash
```

## Step 14 - Pushing to dockerhub
- Once you’ve created your image, you can push it to dockerhub to share it with the world.
  1. Signup to `dockerhub`
  2. Create a new repository
     ![alt text](images/image-12.png)
  3. Login to docker cli
     1. docker login
     2. you might have to create an access token - [Link to Access Token](https://docs.docker.com/security/for-developers/access-tokens/).
  4. Push to the repository
  ```cmd
    docker tag nodejs_project ramchan1/nodejs_project:latest
    docker push your_username/your_reponame:tagname
  ```

