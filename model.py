import pandas as pd
import numpy as np
import joblib
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, accuracy_score

# --- Configuration (Synchronized with Week 7/8 Reports) ---
CSV_FILE = 'execution_trace.csv'
MODEL_PATH = 'jit_predictor_v1.pkl'
WINDOW_SIZE = 5      # Number of previous opcodes to consider
HOT_THRESHOLD = 50   # Threshold for "Hot Spot" identification

def load_and_preprocess(file_path):
    print(f"[*] Reading trace data from {file_path}...")
    df = pd.read_csv(file_path)
    
    # 1. Labeling Logic: 
    # Calculate global frequency per PC to determine ground truth 'hotness'
    pc_counts = df['pc'].value_counts()
    hot_pcs = pc_counts[pc_counts > HOT_THRESHOLD].index.tolist()
    
    # Create binary labels: 1 = HOT (Compilable), 0 = COLD (Interpreter only)
    df['is_hot'] = df['pc'].apply(lambda x: 1 if x in hot_pcs else 0)
    
    print(f"[*] Data Statistics: {len(df)} cycles, {len(hot_pcs)} Hot PCs identified.")

    # 2. Feature Engineering: Sliding Window
    # We create a feature vector: [Current PC, Prev Op 1, Prev Op 2, ... Prev Op 5]
    features = []
    labels = []

    # Iterate through the trace starting after the first window
    for i in range(WINDOW_SIZE, len(df)):
        # Features: [Current PC] + [Last N Opcodes]
        current_pc = df.iloc[i]['pc']
        prev_opcodes = df.iloc[i-WINDOW_SIZE : i]['opcode'].values.tolist()
        
        features.append([current_pc] + prev_opcodes)
        labels.append(df.iloc[i]['is_hot'])

    return np.array(features), np.array(labels)

def train_model():
    # Load data
    try:
        X, y = load_and_preprocess(CSV_FILE)
    except FileNotFoundError:
        print(f"[!] Error: {CSV_FILE} not found. Run your VM first to generate traces.")
        return

    # Split into Training and Testing sets
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )

    # Initialize Random Forest Classifier
    print("[*] Training Random Forest model...")
    clf = RandomForestClassifier(
        n_estimators=100, 
        max_depth=12, 
        random_state=42,
        n_jobs=-1 # Use all CPU cores
    )

    clf.fit(X_train, y_train)

    # Evaluation
    y_pred = clf.predict(X_test)
    print("\n--- Model Evaluation Report ---")
    print(f"Accuracy: {accuracy_score(y_test, y_pred):.4f}")
    print(classification_report(y_test, y_pred, target_names=['COLD', 'HOT']))

    # Save the model for use in the Inference Bridge
    joblib.dump(clf, MODEL_PATH)
    print(f"[*] Model serialized and saved to {MODEL_PATH}")

if __name__ == "__main__":
    train_model()