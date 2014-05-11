Installing modules 
----------------------------------
* Chaos tool suite
 <pre>$drush en ctools -y</pre>
* oAuth ( Me da un error recursivo preguntando sobrescribir el directorio sites/all/modules/oAuth )
 <pre>$drush en oauth -y</pre>
* Libraries (REST Server dependency)
 <pre>$drush en libraries -y</pre>
* Services (includes REST Server)
 <pre>$drush en services -y</pre>
Activar via web oAuth, Services, Services-authentication, services-servers(REST server)

Activating CES Interoperatibily module
--------------------------------------
As you have [installed](https://wiki.enredaos.net/index.php?title=InitIntegralCESDev#Instalando_m.C3.B3dulo_ces_para_desarrollo) an integralCES web site, you need to activate *CES|CES_interop module* via your admin UI interface in **modules** section.


Add Authorization level 
-------------------------------------
Entrar en el panel de **configuración** de nuestro Drupal y en el apartado **Web Services** entramos en **oAuth** y seleccionamos la pestaña *Add Context*. Damos **context title** (*CES Context*) y **context name** (*cesinterop_context*).  Dentro de ésta, en la sección **Authorization Levels**, pulsamos sobre el botón **Add authorization level**, le damos un **nombre** (*interop*) y un **título** (*CES Interoperatibility*) al nuevo nivel y seleccionamos la casilla *Selected By Default*. En **signature methods**, escogemos los métodos de firma que queremos habilitar, para este ejemplo *HMAC_SHA1*.

Create service json 
------------------------------------
Crearemos un servicio con **autorización**: *3-legged oAuth*.

Lo siguiente será **crear un servicio**. Para esto deberemos dirigirnos a *Structure-> Services –> Add*. Como **endpoint** le damos la dirección que seguirá a la dirección raíz de nuestro drupal (http://my_drupal_local/my_instalacion_local/endpoint. Si le damos como endpoint *gateway*, en nuestro caso será: http://cicdev.enredaos.net/cesinterop/gateway. Como **server** *REST* y como **machine name** el que queramos (*ces_gateway*) y le otorgamos **autentificación** *oAuth* marcando la correspondiente casilla.

A continuación, pasaremos a **editar el servicio** recién creado. En la pestaña **Server** sólo dejaremos marcadas las casillas *json* y *application/json*. También podemos agregar *application/x-www-form-urlencoded*  si vamos a enviar formularios.

En **Authentication** seleccionaremos el *contexto oAuth* que hemos creado anteriormente, en Default required Authorization seleccionamos el nivel que le hemos dado a nuestro contexto y en Default required authentication  ponemos  *Consumer key, 3-legged oAuth*. La última pestaña a editar será **Resources**. 

Aquí volveremos tras instalar el resource **ces/ces_interop** para activarlo. En **required authentication** habrá que señalar en el resource **interop**:

- CREATE *Consumer key + token*
- UPDATE *Consumer key + token*
- INDEX *Cap* (la validació es farà al control d'access del módul).

y en **required authorization** de cada uno de ellos, el contexto creado *CES Interoperatibility*.


Create user oAuth consumer 
----------------------------------------
Para terminar sólo tendremos que crear un **usuario oAuth** que nos de acceso al servicio sin poner en riesgo la seguridad del sistema. Para esto, en *gent->permisos->rols* crearemos un **rol de usuario** llamado *ces oAuth* y que para este ejemplo posea los siguientes **privilegios adicionales**: En el apartado **oAuth** hay que añadirle *Access own oAuth authorizations*, *Access own oAuth consumers*, *Register oAuth consumers in CES Context* y *Authorize oAuth consumers in CES Context*. En el apartado **services**, *save file information*.

Creamos un **usuario consumer** llamado *ces_consumer* con **contraseña** *ces_consumer* y **rol** *ces oAuth* que sea el encargado de realizar las peticiones y nos identificamos con él en nuestro Drupal. Entramos en  **User Account** (*Mi cuenta*) (¿**Es posible que haya que crearle una cuenta dentro de un exchange para que pueda acceder**?) accedemos a la pestaña **oAuth Consumers**, añadimos un consumidor con **nombre** *ces_consumer_oAuth*, por ejemplo, y **callback url** en blanco para pasársela vía código cuando la aplicación que consume la api haga la petición. Y ya está. Ahora, una vez creado, podemos acceder a nuestro **consumer key** que tiene la forma *6fkm9oB4wUugyTGfiBR2oxQU6HE8Z8qV* y **consumer secret**, con forma *3rLQiS63Wzsx3fsjWZefCTgtcj3MWw8u* accediendo a la pestaña **oAuth consumers** del  usuario y clicando en **editar** sobre el consumidor creado. Y con esto hemos terminado de crear nuestro servicio web y nuestro usuario para acceder a sus recursos.

Allow authenticated users in your created context 
---------------------------------------------------------------
In order to allow your users to get 3-legged access token, check in People->Permissions **OAuth** options *Register OAuth consumers in CES Context* and *Authorize OAuth consumers in CES Context*.

Testing
--------------------
Para probar que todo funciona correctamente podemos usar un complemento para Firefox llamado **RESTClient**. Para usarlo hacemos click en su icono en la barra de herramientas y, en su ventana añadimos nuestro **usuario oAuth** en la opción **Authentication – >OAuth** del menú superior. Rellenamos el **Consumer key y el Consumer secret**, el resto no hace falta con nuestro tipo de autentificación 2-legged.

Si hemos habilitado en el paso anterior **node**, podemos lanzar una GET con *nuestra url + endopoint + nombre de resource*, por ejemplo, http://mystite/myendpoint/node y, también, lo mismo pero agregando el id de un nodo existente: http://mysite/myendpoint/gateway/node/1
