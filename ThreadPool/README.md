
# Thread Pool

ThreadPool implementation with continuation and work stealing support

## Requirements 
- .NET Framework 4.7.2
- MSBuild >= 16.5

## Build project

Clone the project

```cmd
  git clone -b vladimir_kutuev --single-branch https://github.com/Stanislav-Sartasov/spbu-mm-parallel-programming.git
```

Go to the project directory

```cmd
  cd spbu-mm-parallel-programming\ThreadPool
```

Build

```cmd
  msbuild -t:restore,build -p:RestorePackagesConfig=true
```


## Running Tests

To run tests, run the following command

```cmd
  packages\NUnit.ConsoleRunner.3.12.0\tools\nunit3-console.exe ThreadPoolTest\bin\Debug\ThreadPoolTest.dll
```
