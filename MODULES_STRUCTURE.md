# Structure modulaire de Webserv

## Vue d'ensemble

Le projet webserv a été réorganisé en 3 modules principaux pour une meilleure maintenabilité et clarté du code :

## 📁 Structure des modules

### 🔍 Module GET (`srcs/Router/GET/`)
**Responsabilité**: Gestion des requêtes GET et fichiers statiques

**Fichiers**:
- `RouterGET.cpp` - Classe principale du module GET
- `Router_serveStaticFile.cpp` - Service de fichiers statiques
- `Router_autoindex.cpp` - Génération des listes de répertoires
- `Router_files_utils.cpp` - Utilitaires de gestion des fichiers

**Fonctionnalités**:
- ✅ Service de fichiers statiques (HTML, CSS, images, etc.)
- ✅ Autoindex (listing des répertoires)
- ✅ Gestion des types MIME
- ✅ Vérification de l'existence et permissions des fichiers
- ✅ Gestion de la directive `index`

### 📤 Module POST (`srcs/Router/POST/`)
**Responsabilité**: Gestion des requêtes POST et DELETE

**Fichiers**:
- `RouterPOST.cpp` - Classe principale du module POST
- `Router_methods.cpp` - Implémentation des méthodes POST/DELETE

**Fonctionnalités**:
- ✅ Upload de fichiers
- ✅ Traitement des données POST
- ✅ Suppression de fichiers (DELETE)
- ✅ Validation du Content-Length
- ✅ Gestion des répertoires d'upload

### ⚡ Module CGI (`srcs/Router/CGI/`)
**Responsabilité**: Exécution des scripts CGI

**Fichiers**:
- `RouterCGI.cpp` - Classe principale du module CGI
- `Router_cgi.cpp` - Gestion principale des CGI
- `Router_cgi_env.cpp` - Construction de l'environnement CGI
- `Router_cgi_execute.cpp` - Exécution des processus CGI

**Fonctionnalités**:
- ✅ Exécution de scripts PHP, Python, Shell
- ✅ Gestion de l'environnement CGI (variables)
- ✅ Communication via pipes
- ✅ Timeout et gestion des processus
- ✅ Support des extensions configurables

## 🏗️ Architecture

```
Router (classe principale)
├── RouterGET (module GET)
├── RouterPOST (module POST/DELETE)  
└── RouterCGI (module CGI)
```

### Avantages de cette structure:

1. **Séparation des responsabilités**: Chaque module a un rôle bien défini
2. **Maintenabilité**: Modifications isolées dans chaque module
3. **Lisibilité**: Code plus facile à comprendre et naviguer
4. **Extensibilité**: Ajout facile de nouvelles fonctionnalités
5. **Tests**: Tests unitaires par module

## 🚀 Compilation

Le Makefile a été mis à jour pour supporter la nouvelle structure :

```bash
make        # Compilation complète
make clean  # Nettoyage des objets
make fclean # Nettoyage complet
make re     # Recompilation complète
```

## 🔧 Utilisation

L'interface publique du Router reste identique. La modularité est transparente pour l'utilisateur :

```cpp
Router router(serverConfig);
HttpResponse response = router.route(request);
```

Le Router distribue automatiquement les requêtes aux modules appropriés selon la méthode HTTP et le type de contenu.