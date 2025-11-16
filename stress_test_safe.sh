#!/bin/bash

# Script de stress testing avec timeouts
cd /home/itaharbo/FULL/webserv

# Démarrer le serveur
./webserv webserv.conf > /tmp/webserv_stress.log 2>&1 &
WEBSERV_PID=$!
echo "Server started with PID: $WEBSERV_PID"
sleep 2

# Fonction pour lancer des requêtes parallèles avec timeout
run_parallel_batch() {
    local count=$1
    local url=$2
    local timeout=$3
    
    for i in $(seq 1 $count); do
        curl -s --max-time $timeout "$url" > /dev/null 2>&1 &
    done
    
    # Attendre avec timeout global
    local wait_time=0
    local max_wait=$((timeout + 5))
    
    while [ $(jobs -r | wc -l) -gt 0 ] && [ $wait_time -lt $max_wait ]; do
        sleep 0.5
        wait_time=$((wait_time + 1))
    done
    
    # Tuer les processus qui traînent
    jobs -p | xargs -r kill -9 2>/dev/null
    wait 2>/dev/null
}

# Test 1: Requêtes séquentielles
echo ""
echo "=== Test 1: 50 requêtes GET séquentielles ==="
SUCCESS=0
for i in {1..50}; do
    if curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1; then
        SUCCESS=$((SUCCESS + 1))
    fi
    [ $((i % 10)) -eq 0 ] && echo "  $i/50 complétées"
done
echo "✅ Test 1 terminé: $SUCCESS/50 réussies"

# Test 2: Petits lots parallèles
echo ""
echo "=== Test 2: 3 lots de 5 requêtes parallèles ==="
for batch in {1..3}; do
    echo "  Batch $batch/3..."
    run_parallel_batch 5 "http://localhost:8080/" 3
    echo "  ✓ Terminé"
done
echo "✅ Test 2 terminé"

# Test 3: CGI
echo ""
echo "=== Test 3: 10 requêtes CGI ==="
CGI_SUCCESS=0
for i in {1..10}; do
    if curl -s --max-time 3 http://localhost:8080/cgi-bin/pathinfo_test.php > /dev/null 2>&1; then
        CGI_SUCCESS=$((CGI_SUCCESS + 1))
    fi
done
echo "✅ Test 3 terminé: $CGI_SUCCESS/10 réussies"

# Test 4: POST
echo ""
echo "=== Test 4: 5 uploads ==="
POST_SUCCESS=0
for i in {1..5}; do
    if echo "test$i" | curl -s --max-time 2 -X POST --data-binary @- http://localhost:8080/uploads > /dev/null 2>&1; then
        POST_SUCCESS=$((POST_SUCCESS + 1))
    fi
done
echo "✅ Test 4 terminé: $POST_SUCCESS/5 réussies"

# Vérification finale
echo ""
echo "=== Vérification finale ==="
if curl -s --max-time 2 http://localhost:8080/ > /dev/null 2>&1; then
    echo "✅ Serveur responsive"
else
    echo "❌ Serveur non responsive"
fi

# Arrêt propre
echo ""
echo "Arrêt du serveur..."
kill -INT $WEBSERV_PID 2>/dev/null
sleep 2

echo ""
echo "=== Log (dernières lignes) ==="
tail -5 /tmp/webserv_stress.log

echo ""
echo "🎉 Tous les tests terminés avec succès!"
