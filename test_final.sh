#!/bin/bash

cd /home/itaharbo/FULL/webserv

echo "=== Test final de production ==="
echo ""

# Démarrer le serveur
./webserv webserv.conf > /tmp/webserv_test.log 2>&1 &
WEBSERV_PID=$!
echo "✓ Serveur démarré (PID: $WEBSERV_PID)"
sleep 2

# Test 1: Requêtes séquentielles
echo ""
echo "Test 1: 100 requêtes GET séquentielles"
SUCCESS=0
FAILED=0
for i in {1..100}; do
    if timeout 2 curl -s http://localhost:8080/ > /dev/null 2>&1; then
        SUCCESS=$((SUCCESS + 1))
    else
        FAILED=$((FAILED + 1))
    fi
    [ $((i % 20)) -eq 0 ] && echo "  $i/100 (✓$SUCCESS ✗$FAILED)"
done
echo "✓ Test 1: $SUCCESS/100 réussies"

# Test 2: Requêtes parallèles avec timeout
echo ""
echo "Test 2: 20 requêtes parallèles (4 lots de 5)"
for batch in {1..4}; do
    echo "  Lot $batch/4..."
    for i in {1..5}; do
        (timeout 3 curl -s http://localhost:8080/ > /dev/null 2>&1) &
    done
    # Attendre maximum 5 secondes
    timeout 5 bash -c 'while [ $(jobs -r | wc -l) -gt 0 ]; do sleep 0.2; done'
    # Tuer les processus restants
    jobs -p | xargs -r kill -9 2>/dev/null
    wait 2>/dev/null
done
echo "✓ Test 2: Terminé"

# Test 3: CGI
echo ""
echo "Test 3: 10 requêtes CGI"
CGI_SUCCESS=0
for i in {1..10}; do
    if timeout 5 curl -s http://localhost:8080/cgi-bin/pathinfo_test.php > /dev/null 2>&1; then
        CGI_SUCCESS=$((CGI_SUCCESS + 1))
    fi
done
echo "✓ Test 3: $CGI_SUCCESS/10 réussies"

# Test 4: Vérification finale
echo ""
echo "Test 4: Vérification serveur responsive"
if ps -p $WEBSERV_PID > /dev/null 2>&1; then
    if timeout 2 curl -s http://localhost:8080/ > /dev/null 2>&1; then
        echo "✓ Serveur responsive"
    else
        echo "⚠ Serveur actif mais ne répond pas"
    fi
else
    echo "⚠ Serveur s'est arrêté"
fi

# Arrêt
echo ""
echo "Arrêt du serveur..."
kill -INT $WEBSERV_PID 2>/dev/null
sleep 2

# Vérifier que le serveur s'est arrêté
if ps -p $WEBSERV_PID > /dev/null 2>&1; then
    echo "⚠ Serveur encore actif, force kill"
    kill -9 $WEBSERV_PID 2>/dev/null
else
    echo "✓ Serveur arrêté proprement"
fi

echo ""
echo "=== Résumé ==="
echo "Requêtes séquentielles: $SUCCESS/100"
echo "Requêtes parallèles: Terminé"
echo "Requêtes CGI: $CGI_SUCCESS/10"
echo ""
echo "=== Dernières lignes du log ==="
tail -10 /tmp/webserv_test.log
