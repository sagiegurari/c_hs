## CHANGELOG

### v0.6.1 (2021-10-06)

* Route callbacks are called in the reverse order of entry

### v0.6.0 (2021-10-02)

* Enhancement: New security headers route.
* Enhancement: Move auth routes to security namespace.
* Fix: directory route now returns mime type.
* Fix: memory leak in basic auth route.
* Enhancement: Realm will be released with the basic auth route release
* Enhancement: Make additional params of fs routes released with route
* Enhancement: Refactor server to enable custom connection handling (preparation for multi threaded support)
* Enhancement: Flow params now contains connection state which holds shared data (such as request counter) for all requests on same connection
* Enhancement: New rate limiting routes

### v0.5.1 (2021-09-26)

* Fix: Mime type detection by file name now handles multi case file names.

### v0.5.0 (2021-09-26)

* Fix: Relative href values in FS directory route
* Enhancement: Enable FS file route to close connections to enable multi connection support on single threaded servers
* Enhancement: New redirection route
* Enhancement: New static content routes

### v0.4.0 (2021-09-24)

* Fix: Incorrect handling of binary files
* Enhancement: New options for the hs_routes_fs_file_route_new_with_options function
* Enhancement: New hs_routes_fs_directory_route_new_with_media_support function
* Enhancement: Added additional mime types
* Enhancement: New favicon route

### v0.3.0 (2021-09-09)

* Routes support all HTTP methods.
* 404 route supports all child paths by default.
* New session route (HTTP session).
* Refactor router flow invocation to enable non response routes to modify final HTTP response.
* New simplified api for add/remove/get headers, cookies and state.
* New powered-by route
* Support multiple post response callbacks
* New route flow state which can be used to store data as part of the request handling flow
* Renaming common routes to enable internal refactoring
* Close the socket if payload exists but not read after response is sent.
* New 411 route.
* New payload limit route.

### v0.2.3 (2021-08-24)

* Fix response headers writing
* New basic auth route
* Authorization http request header support
* Adding doctype to directory route html response

### v0.2.2 (2021-08-24)

* Fix directory route - show only file name, not full path

### v0.2.1 (2021-08-24)

* Enable directory route to accept additional head html
* Added classes on directory view to enable customization

### v0.2.0 (2021-08-24)

* Reading payload will limit based on content length header

### v0.1.1 (2021-08-23)

* New post response callback to enable to invoke functionality after response is written
* Enable file route to accept external mime type resolution

### v0.1.0 (2021-08-21)

* Initial release
