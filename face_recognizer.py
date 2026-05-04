#!/usr/bin/env python3
"""
Script de reconnaissance faciale pour CUIREA Qt App
Utilise OpenCV (LBPH) — pas besoin de dlib ni de compilation
Usage:
  python face_recognizer.py register <employee_id> <image_path>
  python face_recognizer.py recognize <image_path>
  python face_recognizer.py check
"""

import sys
import os
import pickle
import numpy as np

def get_faces_dir():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    faces_dir = os.path.join(script_dir, "faces_data")
    os.makedirs(faces_dir, exist_ok=True)
    return faces_dir

def get_model_file():
    return os.path.join(get_faces_dir(), "lbph_model.xml")

def get_labels_file():
    return os.path.join(get_faces_dir(), "labels.pkl")

def load_labels():
    path = get_labels_file()
    if os.path.exists(path):
        with open(path, "rb") as f:
            return pickle.load(f)
    return {}  # {int_label: employee_id}

def save_labels(labels):
    with open(get_labels_file(), "wb") as f:
        pickle.dump(labels, f)

def detect_face(image_gray, face_cascade):
    """Détecte et retourne le visage recadré, ou None."""
    faces = face_cascade.detectMultiScale(
        image_gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(80, 80)
    )
    if len(faces) == 0:
        return None, None
    if len(faces) > 1:
        return None, "MULTIPLE"
    x, y, w, h = faces[0]
    face_roi = image_gray[y:y+h, x:x+w]
    face_roi = cv2.resize(face_roi, (200, 200))
    return face_roi, (x, y, w, h)

def register(employee_id, image_path):
    try:
        import cv2

        if not os.path.exists(image_path):
            print("ERROR:Image non trouvée")
            sys.exit(1)

        face_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
        )

        image = cv2.imread(image_path)
        if image is None:
            print("ERROR:Impossible de lire l'image")
            sys.exit(1)

        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        face_roi, info = detect_face(gray, face_cascade)

        if face_roi is None:
            if info == "MULTIPLE":
                print("MULTIPLE_FACES")
            else:
                print("NO_FACE")
            sys.exit(1)

        emp_id = int(employee_id)

        # Charger les labels existants
        labels = load_labels()

        # Trouver ou créer un label entier pour cet employé
        # LBPH nécessite des labels entiers
        reverse = {v: k for k, v in labels.items()}
        if emp_id in reverse:
            int_label = reverse[emp_id]
        else:
            int_label = len(labels)
            labels[int_label] = emp_id

        # Charger ou créer le modèle LBPH
        model_path = get_model_file()
        recognizer = cv2.face.LBPHFaceRecognizer_create()

        if os.path.exists(model_path):
            recognizer.read(model_path)
            # Mettre à jour avec le nouveau visage
            recognizer.update([face_roi], np.array([int_label]))
        else:
            # Premier enregistrement
            recognizer.train([face_roi], np.array([int_label]))

        recognizer.save(model_path)
        save_labels(labels)

        print(f"SUCCESS:{employee_id}")
        sys.exit(0)

    except ImportError as e:
        print(f"ERROR:OpenCV non installé: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"ERROR:{str(e)}")
        sys.exit(1)

def recognize(image_path):
    try:
        import cv2

        model_path = get_model_file()
        if not os.path.exists(model_path):
            print("ERROR:Aucun visage enregistré")
            sys.exit(1)

        if not os.path.exists(image_path):
            print("ERROR:Image non trouvée")
            sys.exit(1)

        face_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
        )

        image = cv2.imread(image_path)
        if image is None:
            print("ERROR:Impossible de lire l'image")
            sys.exit(1)

        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        face_roi, info = detect_face(gray, face_cascade)

        if face_roi is None:
            if info == "MULTIPLE":
                print("MULTIPLE_FACES")
            else:
                print("NO_FACE")
            sys.exit(1)

        # Charger le modèle et prédire
        recognizer = cv2.face.LBPHFaceRecognizer_create()
        recognizer.read(model_path)

        int_label, confidence = recognizer.predict(face_roi)

        # LBPH: confidence < 50 = très bon, < 80 = acceptable, > 100 = inconnu
        THRESHOLD = 85.0

        labels = load_labels()
        employee_id = labels.get(int_label, -1)

        if confidence <= THRESHOLD and employee_id != -1:
            # Convertir confidence en pourcentage lisible (0=parfait, 100=mauvais)
            score = max(0, int(100 - confidence))
            print(f"RECOGNIZED:{employee_id}:{score}")
            sys.exit(0)
        else:
            print("UNKNOWN")
            sys.exit(1)

    except ImportError as e:
        print(f"ERROR:OpenCV non installé: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"ERROR:{str(e)}")
        sys.exit(1)

def check():
    try:
        import cv2
        # Vérifier que le module face est disponible
        _ = cv2.face.LBPHFaceRecognizer_create()
        print("AVAILABLE")
        sys.exit(0)
    except ImportError:
        print("NOT_AVAILABLE:opencv-python non installé")
        sys.exit(1)
    except AttributeError:
        print("NOT_AVAILABLE:opencv-contrib-python requis")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: face_recognizer.py <register|recognize|check> [args...]")
        sys.exit(1)

    command = sys.argv[1].lower()

    if command == "register":
        if len(sys.argv) < 4:
            print("ERROR:Arguments manquants: register <id> <image_path>")
            sys.exit(1)
        register(sys.argv[2], sys.argv[3])

    elif command == "recognize":
        if len(sys.argv) < 3:
            print("ERROR:Arguments manquants: recognize <image_path>")
            sys.exit(1)
        recognize(sys.argv[2])

    elif command == "check":
        check()

    else:
        print(f"ERROR:Commande inconnue: {command}")
        sys.exit(1)
