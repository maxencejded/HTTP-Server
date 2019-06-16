# HTTP Server in C

This project is a C HTTP Server in C.

## Getting Started

The goal of this project is to understand the HTTP protocol and his specificity by providing a whole Server in C using only the standard C libraries.

### Installing

To create the server just go to the root of the folder and:

```bash
$ git clone https://github.com/maxencejded/HTTP-Server.git server
$ cd server
$ make
$ ./server
Starting new Server
Adrress: 0.0.0.0:6060
```

If you want to add your website, you'll be expected to insert it in the `site` folder. The `index.html` should be at the root of the folder.

### API
If you are willing to add API's, you'll have to create them in the API folder:

```bash
$ cd api/
```
The demo file is an api to show you how you should format your content.
Don't forget that you will have to print the HTTP header by yourself.
Finally, you are expected to add the name in the API array. Don't forget to increase the size of the number of API_COUNT.

```bash
$ vim includes/api.h
```
```C
# define API_COUNT 1 /* NUMBER OF APIs */

int        demo(t_http *request, t_reponse *answer);
/* Add your functions here */
```

## Running Test

Tests will be available soon. They are being written.

## Deployment

Compatible on macOS (`darwin x86_64`).\
We are currently working on making it available for any UNIX systems.

## Built With

* [GCC](https://gcc.gnu.org/) - Compilator
* [42 Norme](https://github.com/Nqsir/The-Norm-42/blob/master/norme.fr.pdf) - Norme for C

## Authors

* **Jules Spender** - *Initial work* - [spenderj](https://github.com/spenderj)
* **Maxence Jacques de Dixmude** - *Initial work* - [maxencejded](https://github.com/maxencejded)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

HTTP protocol, Parsing, Dynamic memory management, Optimization, Debugging, Error management

