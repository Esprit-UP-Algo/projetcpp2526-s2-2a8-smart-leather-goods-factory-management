#!/usr/bin/env python3
"""
Script Python pour la reconnaissance faciale
Utilise la bibliothèque face_recognition
"""

import sys
import os
import pickle
import face_recognition
import numpy as np
from pathlib import Path

# Dossier pour stocker les encodages
ENCODINGS_FILE = "faces_data/face_encodings.pkl"

def load_encodings():
    """Charger les encodages de visages depuis le fichier"""
    if os.path.exists(ENCODINGS_FILE):
        try:
            with open(ENCODINGS_FILE, 'rb') as f:
                return pickle.load(f)
        except Exception as e:
            print(f"ERROR: Erreur chargement encodages: {e}", file=sys.stderr)
            return {}
    return {}

def save_encodings(encodings):
    """Sauvegarder les encodages de visages dans le fichier"""
    try:
        # Créer le dossier si nécessaire
        os.makedirs(os.path.dirname(ENCODINGS_FILE), exist_ok=True)
        
        with open(ENCODINGS_FILE, 'wb') as f:
            pickle.dump(encodings, f)
        return True
    except Exception as e:
        print(f"ERROR: Erreur sauvegarde encodages: {e}", file=sys.stderr)
        return False

def register_face(employee_id, image_path):
    """Enregistrer un nouveau visage"""
    try:
        # Charger l'image
        image = face_recognition.load_image_file(image_path)
        
        # Détecter les visages
        face_locations = face_recognition.face_locations(image)
        
        if len(face_locations) == 0:
            print("NO_FACE: Aucun visage détecté")
            return False
        
        if len(face_locations) > 1:
            print("MULTIPLE_FACES: Plusieurs visages détectés")
            return False
        
        # Encoder le visage
        face_encodings = face_recognition.face_encodings(image, face_locations)
        
        if len(face_encodings) == 0:
            print("ERROR: Impossible d'encoder le visage")
            return False
        
        # Charger les encodages existants
        encodings = load_encodings()
        
        # Ajouter le nouveau visage
        encodings[int(employee_id)] = face_encodings[0]
        
        # Sauvegarder
        if save_encodings(encodings):
            print(f"SUCCESS: Visage enregistré pour l'employé {employee_id}")
            return True
        else:
            print("ERROR: Erreur lors de la sauvegarde")
            return False
            
    except Exception as e:
        print(f"ERROR: {str(e)}", file=sys.stderr)
        return False

def recognize_face(image_path):
    """Reconnaître un visage"""
    try:
        # Charger les encodages
        encodings = load_encodings()
        
        if not encodings:
            print("ERROR: Aucun visage enregistré")
            return -1
        
        # Charger l'image à reconnaître
        image = face_recognition.load_image_file(image_path)
        
        # Détecter les visages
        face_locations = face_recognition.face_locations(image)
        
        if len(face_locations) == 0:
            print("NO_FACE: Aucun visage détecté")
            return -1
        
        # Encoder le visage
        face_encodings = face_recognition.face_encodings(image, face_locations)
        
        if len(face_encodings) == 0:
            print("ERROR: Impossible d'encoder le visage")
            return -1
        
        unknown_encoding = face_encodings[0]
        
        # Comparer avec tous les visages enregistrés
        best_match_id = -1
        best_distance = 1.0  # Distance maximale
        
        for employee_id, known_encoding in encodings.items():
            # Calculer la distance
            face_distance = face_recognition.face_distance([known_encoding], unknown_encoding)[0]
            
            # Si c'est une meilleure correspondance
            if face_distance < best_distance and face_distance < 0.6:  # Seuil de 0.6
                best_distance = face_distance
                best_match_id = employee_id
        
        if best_match_id != -1:
            print(f"RECOGNIZED: {best_match_id}")
            print(f"CONFIDENCE: {(1 - best_distance) * 100:.2f}%", file=sys.stderr)
            return best_match_id
        else:
            print("UNKNOWN: Visage non reconnu")
            return -1
            
    except Exception as e:
        print(f"ERROR: {str(e)}", file=sys.stderr)
        return -1

def main():
    if len(sys.argv) < 2:
        print("Usage: python face_recognizer.py <command> [args]")
        print("Commands:")
        print("  register <employee_id> <image_path>")
        print("  recognize <image_path>")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == "register":
        if len(sys.argv) != 4:
            print("Usage: python face_recognizer.py register <employee_id> <image_path>")
            sys.exit(1)
        
        employee_id = sys.argv[2]
        image_path = sys.argv[3]
        
        success = register_face(employee_id, image_path)
        sys.exit(0 if success else 1)
        
    elif command == "recognize":
        if len(sys.argv) != 3:
            print("Usage: python face_recognizer.py recognize <image_path>")
            sys.exit(1)
        
        image_path = sys.argv[2]
        
        employee_id = recognize_face(image_path)
        sys.exit(0 if employee_id != -1 else 1)
        
    else:
        print(f"Commande inconnue: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
