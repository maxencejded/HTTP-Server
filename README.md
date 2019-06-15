# HTTP Server in C

This project is a C HTTP Server in C.

## Getting Started

The goal of this project is to provide a whole HTTP Server in C.

### Prerequisites

No library are required to launch this project. The server only uses C libraries.

### Installing

To create the server just go to the root of the folder and:

```bash
Make
```

If you want to add your website, you'll be expected to insert it in the site folder.

```bash
rm -rf site/*
```
Then just copy the content of your website in this folder with index.html at the root of the folder.

If you are willing to add APIS, you'll have to create them in the API folder:

```bash
cd api/
```
The demo file is an api to show you how you should format your content.
Don't forget that you will have to print the HTTP header by yourself.
Finally, you are expected to add the name in the API array. Don't forget to increase the size of the number of API_COUNT.

```bash
vim includes/api.h
```
```C
# define API_COUNT 1 (NUMBER OF APIS)

int	demo(t_http *request, t_reponse *answer);
# After this line you are expected to add your function calls there
```

Then

```bash
vim srcs/response/response.c
```
```C
t_api				g_api[API_COUNT] = {
	{"demo", &demo}
  # Add your name of function there
};
```

## Running the tests

Tests will be available soon. They are being written.

### Break down into end to end tests

Will be soon released

```bash
sh test.sh
```

### And coding style tests

Bash unit tester are coded this way

```
Test examples
```

## Deployment

This C server is compatible on MAC OS.
We are currently working on making it available for any UNIX system.

## Built With

* [GCC](https://gcc.gnu.org/) - Used Compilator
* [42 Norme](https://github.com/Nqsir/The-Norm-42/blob/master/norme.fr.pdf) - Norme for C

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/maxencejded/HTTP-Server/contributing.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Maxence Jacques** - *Initial work* - [maxencejded](https://github.com/maxencejded)
* **Jules Spender** - *Initial work* - [spenderj](https://github.com/spenderj)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* HTTP format
* Signal
