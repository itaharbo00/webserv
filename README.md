# webserv

**42 project**: an HTTP server implemented in **C++98**.

The `webserv` binary parses an NGINX-like configuration file, opens one listening socket per unique port, then handles connections in non-blocking mode using `poll()`.

## Features (implemented)

- **HTTP/1.0** and **HTTP/1.1**
- Methods: **GET**, **HEAD**, **POST**, **DELETE**
- Static file serving (basic MIME types)
- **Autoindex** (directory listing) configurable at `server` or `location` level
- Custom **error pages** via `error_page`
- **CGI** via `cgi_pass` (sync + async execution)
- **Uploads** on `POST`: writes the request body into `./uploads` (or `upload_store` if configured)
- Redirects via `return <3xx> <url>`

## Build

```bash
make
```

The output binary is `./webserv`.

## Run

```bash
./webserv
```

By default, the server loads `webserv.conf`.

You can also provide a config file:

```bash
./webserv webserv.conf
```

## Configuration

The configuration uses `server { ... }` and `location <path> { ... }` blocks.

### `server` directives

- `listen <port>;`
- `server_name <hostname>;`
- `root <path>;`
- `index <file>;`
- `autoindex on|off;`
- `client_max_body_size <size>;` (e.g. `10M`, `120M`, `1G`)
- `error_page <code> <path>;` (can contain multiple codes on one line)
- `location <path> { ... }`

### `location` directives

- `allow_methods GET POST DELETE HEAD;`
- `index <file>;` (overrides server `index`)
- `autoindex on|off;` (overrides server `autoindex` if explicitly set)
- `client_max_body_size <size>;` (takes priority over server value if > 0)
- `upload_store <path>;` (upload directory; if not set, uploads go to `./uploads`)
- `cgi_pass <.ext> <cgi_path>;` (e.g. `.py /usr/bin/python3`)
- `return <3xx> <url>;`

### Virtual hosts (server_name)

If multiple `server` blocks share the same `listen` port, selection is done using the `Host` header.
The matching is **strict** on the hostname (the port in `Host` is ignored).

## Config example (from this repo)

The file [webserv.conf](webserv.conf) starts a server on `8080` and serves from `./www`.

Excerpt:

```conf
server {
	listen 8080;
	server_name localhost;
	root ./www;

	error_page 404 /errors/404.html;
	error_page 500 /errors/50x.html;

	location / {
		index index.html;
		allow_methods GET POST HEAD;
	}

	location /cgi-bin {
		allow_methods GET POST HEAD;
		cgi_pass .py /usr/bin/python3;
		cgi_pass .php /usr/bin/php-cgi;
		cgi_pass .sh /bin/bash;
	}

	location /upload {
		client_max_body_size 120M;
		allow_methods GET POST DELETE;
	}
}
```

## Quick tests (curl)

### GET / HEAD

```bash
curl -i http://localhost:8080/
curl -I http://localhost:8080/
```

### Autoindex

In [webserv_reference.conf](webserv_reference.conf), `autoindex on;` is enabled for `/files`.

```bash
./webserv webserv_reference.conf
curl -i http://localhost:8080/files/
```

### Upload (POST)

The server accepts:

- `Content-Length` **or** `Transfer-Encoding: chunked` (otherwise `411 Length Required`)
- `multipart/form-data` (extracts the first file part)

Multipart upload:

```bash
curl -i -F "file=@./www/index.html" http://localhost:8080/upload
```

Expected: `201 Created` with `Location: http://<host>/uploads/<name>`.

Raw upload (non-multipart):

```bash
curl -i --data-binary @./www/index.html http://localhost:8080/upload
```

In this case, the file is stored as `upload_<timestamp>.bin`.

### DELETE

Delete a file from `./uploads`:

```bash
curl -i -X DELETE http://localhost:8080/uploads/<file>
```

Expected: `204 No Content`.

## CGI

A CGI runs if:

- the `location` matches the URI,
- the location has at least one `cgi_pass`,
- the requested file extension (e.g. `.py`, `.php`, `.sh`) is declared.

Examples:

```bash
curl -i http://localhost:8080/cgi-bin/hello.py
curl -i -X POST --data "name=42" http://localhost:8080/cgi-bin/hello.py
```

Notes:

- The server supports `Status:` and `Location:` headers in CGI output.
- CGI execution timeout is ~30s.

## Limits / implementation notes

- Path security: rejects `..`, `~`, and `//` in the URI.
- Max **header line** size: 8KB.
- Max **request** size (raw buffer): 200MB.
- GET/HEAD refuses to serve files larger than **10MB** (hard-coded limit).
- Autoindex inherits from `server` if the `location` does not explicitly set `autoindex`.
- Multipart upload extracts only the **first part**.

## Repository structure

- [includes/](includes): headers
- [srcs/](srcs): implementation (Config, Server, Router, HttpRequest/Response)
- [www/](www): demo static files + error pages + cgi-bin scripts
- [uploads/](uploads): upload directory (may be created automatically)

## Authors

See Git history.
