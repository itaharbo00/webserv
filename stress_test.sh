#!/bin/bash

# Script de stress testing plus robuste
cd /home/itaharbo/FULL/webserv

# Démarrer le serveur
./webserv webserv.conf > /tmp/webserv_stress.log 2>&1 &
WEBSERV_PID=$!
echo "Server started with PID: $WEBSERV_PID"
sleep 2

# Test 1: Requêtes séquentielles rapides (plus fiable)
echo ""
echo "=== Test 1: 100 requêtes GET séquentielles rapides ==="
SUCCESS=0
FAILED=0
for i in {1..100}; do
    if curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1; then
        SUCCESS=$((SUCCESS + 1))
    else
        FAILED=$((FAILED + 1))
    fi
    # Afficher progression tous les 10
    if [ $((i % 10)) -eq 0 ]; then
        echo "  Progress: $i/100 (Success: $SUCCESS, Failed: $FAILED)"
    fi
done
echo "✅ Test 1 terminé: $SUCCESS réussies, $FAILED échouées"

# Test 2: Petits lots de requêtes parallèles (plus contrôlable)
echo ""
echo "=== Test 2: 5 lots de 10 requêtes parallèles ==="
for batch in {1..5}; do
    echo "  Batch $batch/5..."
    for i in {1..10}; do
        curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1 &
    done
    wait
    echo "  ✓ Batch $batch terminé"
done
echo "✅ Test 2 terminé"

# Test 3: Requêtes CGI
echo ""
echo "=== Test 3: 20 requêtes CGI ==="
CGI_SUCCESS=0
CGI_FAILED=0
for i in {1..20}; do
    if curl -s --max-time 3 http://localhost:8080/cgi-bin/pathinfo_test.php > /dev/null 2>&1; then
        CGI_SUCCESS=$((CGI_SUCCESS + 1))
    else
        CGI_FAILED=$((CGI_FAILED + 1))
    fi
done
echo "✅ Test 3 terminé: $CGI_SUCCESS réussies, $CGI_FAILED échouées"

# Test 4: Mix de requêtes en petits lots
echo ""
echo "=== Test 4: Mix de requêtes (GET + CGI + POST) ==="
for i in {1..5}; do
    curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1 &
    curl -s --max-time 3 http://localhost:8080/cgi-bin/pathinfo_test.php > /dev/null 2>&1 &
    echo "test$i" | curl -s --max-time 2 -X POST --data-binary @- http://localhost:8080/uploads > /dev/null 2>&1 &
done
wait
echo "✅ Test 4 terminé"

# Vérifier que le serveur est toujours responsive
echo ""
echo "=== Test final: Vérification du serveur ==="
if curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1; then
    echo "✅ Serveur toujours responsive après stress test"
else
    echo "❌ Serveur non responsive"
fi

# Arrêter proprement le serveur
echo ""
echo "Arrêt du serveur..."
kill -INT $WEBSERV_PID 2>/dev/null
sleep 2

# Vérifier les logs
echo ""
echo "=== Dernières lignes du log ==="
tail -10 /tmp/webserv_stress.log

echo ""
echo "🎉 Stress test terminé!"
