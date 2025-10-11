# Continuing CI, Certificate Management and Testing

## 1. Deploying a monorepo (HTTP Server, Web Socket, Prisma, Postgres, NextJS Server)
- Initialize a monorepo and deploy it to the cluster, not deploying to the EC2 deploy it somewhere else.
![alt text](image.png)
- We will be using the `pnpm` instead of the `npm` to make our life more difficult.
- There are more package managers like npm, pnpm, bun, yarn etc.
- Install the `pnpm` locally using the command:
```cmd
    npm install -g pnpm
```

- Start building the application.
```cmd
    npx create-turbo@latest
```
- Turborepo is a framework which allows to build a monorepo on the top of it.


## 2. Adding Backend Services
- We will be adding two services(directory) i.e. http-server and ws-server in the apps folder, i.e. http server and the web socket server.

## 3. Adding the ORM(Object Relation Mapping) ft. prisma.
- Prisma is the ORM which deals with the Database operations in the PostgreSQL.
- We will now be adding the prisma(directory) in the packages directory.
- We will build this first and the we will add the http-server and the web-server to this.
```cmd
    cd packages/
    mkdir prisma
    cd packages/prisma/

```

- We will be creating a `package.json` file in the `prisma/` using the command.

```cmd
    npm init -y
```

- Now we will be adding a `tsconfig.json` to the `prisma/` directory using the command.

```cmd
    npx tsc --init
```

- In the `tsconfig.json` we extended the base.json to get all the properties of that file.
```js
{
  "extends": "@repo/typescript-config/base.json"
}
```

- Add the `devDependencies` in the prisma's `package.json` file.
```js
{
  "name": "@repo/db",
  "version": "1.0.0",
  "main": "index.js",
  "keywords": [],
  "author": "",
  "devDependencies": {
    "@repo/typescript-config": "workspace:*"
  },
  "license": "ISC",
  "description": "",
  "dependencies": {
    "prisma": "^6.17.1"
  }
}
```

- In the root package manager i.e. `package.json` we have the `pnpm` package manager.
- Add the prisma dependency to the prisma package.
```cmd
    pnpm add prisma
```

- ORM makes the database connection simpler.
- The schema.prisma file will be created in the `prisma/` directory using the command.
```cmd
    npx prisma init
```

- Now create the Schema in the `schema.prisma`.
```prisma
model User{
  id  String  @id   @default(uuid())
  username  String  @unique
  password  String
}
```

- Now we will get the database from the [Neon Tech](https://neon.com/).
- Signup and Create a database named `class-testing`, choose any cloud provider.
- Copy the database URL using the connection string option and paste it in the `.env` file.
- To connect the prisma to the Neon Tech DB.
```cmd
    npx prisma migrate dev
```

- Now create the table in the database using the `create_users_table` command.
- It will also create a `migration.sql` in the `prisma/migrations/...create_users.table/migration.sql`.

- Now we will have to export the `prisma client` so that the servers like `http-server` and the `web-server` will be able to import them.
- Create the directory in the `prisma/src/index.ts`.
- Now create an `exports` field in the package.json file to export the client.

## 4. NextJS Application:
- Now we can import the client from the page.tsx file from the `web/` directory which is the NextJS Application.
- In the `apps/web/app/page.tsx` import the client like:
```js
  import { client } from "@repo/db/client";
```

- In the dub.sh they have`packages/prisma/package.json` they export `client` from the `client.ts`.
- In the NextJS Server's package.json add the `@repo/db` devdependency.
- This will allow us to use the dependency to interact with the database in the NextJS Application.
- Now the import will still not work because we have only written the dependency in the package.json.
- We have not done the `pnpm install`. Doing the installation on the global level will let the packages install on sublevel as well.
- Now we can add use the client object in the page.tsx file.
```js
  client.user.findOne()
```
- Till now we have only made the application not deployment.

## 5. Create the Backend services:
- In the `http-server/` we will be creating the backend services.
```js
  npm init -y // Create the package.json
  npx tsc --init // Create the tsconfig.json
```

- In the `tsconfig.json` we will be writing a list of objects.
```js
{
  "extends":"@repo/typescript-config/base.json",
  "compilerOptions": {
    "rootDir": "./src",
    "outDir": "./dist"
  }
}
```

- The source code will be present in the `/src` folder.
- The output of the code will be present in the `/dist` folder.
- Add the devDependencies in the `package.json` file in the http-server. 
```js
  "devDependencies": {
    "@repo/db": "workspace:*",
    "@repo/typescript-config": "workspace:*"
  },
```
- Add the build scripts in the `package.json` file.
```js
  "scripts": {
    "build": "tsc -b",
    "dev": "npm run build && npm run start",
    "start": "node dist/index.js"
  },
```
- Do the same thing in the ws-server.
- Now write the source code.
- Add `express` as a dependency in the http-server.
```cmd
  npm install express @types/express
```

## 6. Execute the application
- To execute the application with the following ports:
- **NextJS Application** 
  - It will be executed using the following command
  - `npm run dev`
  - The port number can be changed from the `package.json` file.
- **http-server**
  - It will also be executed using the same command as listed above.
  - It will run on the port number mentioned in the index.ts file.
- **ws-server**
  - It will also be executed using the same command as listed above.
  - It will run on the port number mentioned in the index.ts file.
