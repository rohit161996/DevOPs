# CI/CD Pipelines
- Continous Integration and Continous Development

## Process Management
- Keeps apps running continuously.
- Automatically restarts crashed apps.
- Provides monitoring and logging.
- Supports running multiple app instances for better performance.

## Installation of pm2
- [pm2 Documentation](https://pm2.keymetrics.io/)

## Deploying the Backend
### 1. Create a file called app.js
```js
const express = require("express");
const app = express();

app.get("/", (req, res) => {
  res.send("Hello, World!");
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
```

### 2. Run the following command
```js
pm2 start app.js
```

### Terms to understand
- curl: similar to postman but for terminal
- lsof: this gives you the process id for a given port


## **CI/CD**

### **Continuous Integration**
Continuous Integration (CI) is a development practice where developers frequently integrate their code changes into a shared repository, preferably several times a day. Each integration is automatically verified by

1. Building the project and
2. Running automated tests.

This process allows teams to detect problems early, improve software quality, and reduce the time it takes to validate and release new software updates.

### **Continuous Deployment**
As the name suggests, deploying your code `continuously` to various environments (dev/stage/prod)