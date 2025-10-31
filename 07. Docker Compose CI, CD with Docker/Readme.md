# Docker Compose and CI/CD with Docker
## Docker Compose
- It composes different docker files or images.

- Docker Compose is a `tool` designed to help you `define` and `run` `multi-container` Docker applications.

- With Compose, you use a YAML(.yml) file to configure your application's services, networks, and volumes.

- Then, with a `single command`, you can `create and start all` the `services` from your configuration.

- Docker compose is not used when we are deploying the application, it is mostly used for `local development`.

- Most of the projects have the `docker-compose.yml` file in it, some projects have the file in the new repository all together.

- In the .yml file the `mongo` image is taken from the `docker hub registry` and the backend source code and the front end source code is in the repository which is on the github.

- We use the development docker-compose file for development. 

- The command to run the docker-compose file is:
```cmd
    docker-compose -f docker-compose-development.yml up
```

- In the file -> [p5.js-web-editor](https://github.com/processing/p5.js-web-editor/blob/develop/docker-compose-development.yml) Docker file is somewhere else the compose file starts the mongo service.

<img src="image.png" width="750px" height= "500px">

- The above file defines several things:
  - 2 services -> mongodb, backend.
  - Volumes are mounted on the database.
  - The backend service is dependent on the mongodb, i.e. the backend will not execute if the mongodb is not installed.
  - The equivalent command for the mongo service is:
```cmd
        docker run mongo --name mongodb -p 27017:27017 -v mongo_data:/data/db
```
  - The equivalent command for the backend service is:
```cmd
        docker build -t backend22 .
```

- In the docker compose we do not have to do the network configurations manually it is done automatically.

- We give the URL in the environment variable.

- There are 2 sections of the installation of the github repo locally.
 
### Before docker-compose
- Create a network

## CI/CD Pipelines
- Productionizing a monorepo on the Virtual Machine using the CI/CD pipeline.
- Github repository will also be required with complex monorepo 2 front end and 5 backends.

### Functions of the CI/CD Pipelines:
- The function of the CI/CD Pipeline will be to deploy from [github -> dockerhub -> virtual machine]

## Simple Backend to without monorepo
1. Initalize an empty backend project

```cmd
    mkdir Simple_Project_Without_Monorepo
    cd Simple_Project_Without_Monorepo
```

2. Create the package.json using the command
```cmd
    npm init -y
```

3. Install the typescript package and create the `tsconfig.json` file using the command
```cmd
    npm install typescript
```

```cmd
    npx tsc --init
```

4. In the `tsconfig.json` umcomment the `src` and the `out` dir lines.
```js
    // File Layout
    "rootDir": "./src",
    "outDir": "./dist",
```
5. Create the `src/` and `index.ts` in it.
```js
    import express from "express";
    import { PrismaClient } from "@prisma/client";

    const app = express();
    const prismaClient = new PrismaClient();

    /* API to get the data */
    app.get("/", async (req, res) => {
        const users = await prismaClient.user.findMany();
        res.json({ users });
    })

    /* API to post the data or store the data */
    app.post("/", async (req, res) => {

        await prismaClient.user.create({
            data: {
                username: Math.random().toString(),
                password: Math.random().toString()
            }
        })

        res.json({
            "message": "Post Endpoint"
        })

    })

    app.listen(3000);
```

6. Install the prisma package and other packages because this is the backend project which connects to the prisma database.
```cmd
    npm install prisma express @types/express
```

7. Now initialize the prisma to create the ORMs, this will create a prisma folder in the repo.
```cmd
    npx prisma init
```

8. In the index.ts create two apis to read the data and write the data.

9. In the `schema.prisma` file add a new model named User.
```prisma
    model User{
        id        String      @id       @default(uuid())
        username  String
        password  String
    }
```

10. Now, we have to migrate the database to create the tables in it, but we do not have a database so we will build a database first.

11. Create and Run the database locally using the command:
```cmd
    docker run
        -e POSTGRES_PASSWORD=mysecretpassword # Environment Variable
        -d                                    # Detach Mode
        -p 5432:5432                          # Port Mapping
        postgres                              # Database Name

    docker run 
        -e DATABASE_URL=postgresql://postgres:mysecretpassword@postgres:5432/postgres # Environment Variable
        --network user_project                                                        # Detach Mode
        -p 3000:3000                                                                  # Port Mapping
        user-project                                                                  # Container name
```

12. Change the URL in the `.env` file.
    - postgres is the username.
    - mysecretpassword is the password.
    - locahost is because we are running it locally.
    - postgres is the name of the database.
```js
    DATABASE_URL="postgresql://postgres:mysecretpassword@localhost:5432/postgres"
```

12.  Now migrate the database, it creates a migration directory with the create table query.
```cmd
    npx prisma migrate dev
```
 
13. We can close the database as well after this.

14.  Now we will generate the client using the command
```cmd
    npx prisma generate
```

15. We can now add the CRUD functionality in the app.
    - We will be adding the code to the `index.ts`

16. Now we will be creating a Contribution.md file which will be having all the steps to install the project manually as well as through docker to make contribution.

17. Now add the build and run command to the `package.json` file to build and run the project.
```cmd
  "scripts": {
    "build": "tsc -b",
    "start": "node dist/index.ts",
    "test": "echo \"Error: no test specified\" && exit 1"
  },
```

18. Using the Docker to install the project the steps will shrink down to a less number, which will reduce to 1-2 steps when we will be using the docker compose method.

19. There are few advantages of executing the project locally for e.g. hot reloading.

20. Now we will be creating a docker files in the repo so we will be creating the i.e. Dockerfile and docker-compose.yml file in the project.
```Dockerfile
    FROM node:20-alpine

    WORKDIR /app

    COPY ./package.json ./package.json
    COPY ./package-lock.json ./package-lock.json

    RUN npm install

    COPY . .

    ENV DATABASE_URL=postgresql://postgres:mysecretpassword@postgres:5432/postgres 

    RUN npx prisma generate
    RUN npm run build

    EXPOSE 3000

    CMD ["npm", "start"]
```

21. When we are building the docker image, we are building it in the host machine. The name of the network should be passed as the host.
```Dockerfile
    docker build --network=user_project -t user_project .
```

22. To connect the backend service to the container we need to create a network.

23. Prisma's `schema.prisma` file changed.

24. Now the application can be checked at [Localhost](http://localhost:3000/)

25. Data can be sent using postgres.

### Why did we change our build command?
- docker build 
  --network=host 
  -t user-project .

- Without the argument in the build `--network=host`, the container which was building was pointing to itself for the postgres.
- The postgres is not running on the container at the address `localhost:5432` on the container, postgres is running on the old container.

<img src="image-1.png" height= "450px" width="1000px">

- Now, when we are pointing to the host network.

<img src="image-2.png" height= "450px" width="1000px">

## Now we will be creating the docker-compose file.
- We will be deriving the inspiration from the [Docker File of the p5 web editor](https://github.com/processing/p5.js-web-editor/blob/develop/docker-compose.yml)

- Mention the following things in the docker-compose file.
  - The version of the docker-compose.
  - The services we want in our project.
  - The images in the project.
  - The port number of the services.

- We do not migrate the database in the docker file, we do it in the package.json using a simple command.

```Dockerfile
    FROM node:20-alpine

    WORKDIR /app

    COPY ./package.json ./package.json
    COPY ./package-lock.json ./package-lock.json

    RUN npm install

    COPY . .

    RUN npx prisma generate
    RUN npm run build

    EXPOSE 3000

    CMD ["npm run dev:docker"]
```

```Dockerfile
    version: '3.8'
    services:
    postgres:
        image: postgres
        ports:
        - 5432:5432
        environment:
        - POSTGRES_PASSWORD=mysecretpassword
    
    user_app:
        build:
        network: host
        context: ./ 
        dockerfile: Dockerfile
        
        environment:
        - DATABASE_URL=postgresql://postgres:mysecretpassword@postgres:5432/postgres
    
        ports:
        - 3000:3000
        depends_on:
        - postgres
```