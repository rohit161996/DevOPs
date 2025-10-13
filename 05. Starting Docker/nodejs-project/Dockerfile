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