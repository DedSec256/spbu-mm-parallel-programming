
# Producers and Consumers

Solution of the simplified producer-consumer problem (buffer is unbounded)

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
  cd spbu-mm-parallel-programming\ProducersConsumers
```

Build

```cmd
  msbuild -t:restore,build -p:RestorePackagesConfig=true
```


## Run Locally

Run sample program

```cmd
  ProducersConsumers\bin\Debug\ProducersConsumers.exe <Producers count> <Consumers count>
```


## Running Tests

To run tests, run the following command

```cmd
  packages\NUnit.ConsoleRunner.3.12.0\tools\nunit3-console.exe ProducersConsumersTest\bin\Debug\ProducersConsumersTest.dll
```

  