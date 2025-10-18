# Build a BASE IMAGE in the Docker File
FROM node:22-alpine

# Make app as the working directory
WORKDIR /app

# Copy the package files in the working directory
COPY package* ./

# Install the packages in the working directory
RUN npm install

# Copy the other files to the working directory (Source Code)
COPY . .

# Build the project
RUN npm run build

# Expose the Port Number of the Container Application
EXPOSE 3000

# Run the final command
CMD ["node", "dist/index.js"]