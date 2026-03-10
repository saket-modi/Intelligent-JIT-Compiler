import pandas as pd
import numpy as np
import joblib
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split

def train_generalized_model():
    print("[*] Loading master_trace.csv...")
    df = pd.read_csv('master_trace.csv')
    print(f"[*] Dataset loaded: {len(df)} rows.")

    # 1. Labeling: Keep the PC only to define "Hotness"
    # We define "Hot" as any instruction that occurs more than the threshold
    threshold = 50 
    pc_counts = df['pc'].value_counts()
    hot_pcs = pc_counts[pc_counts > threshold].index.tolist()
    
    df['is_hot'] = df['pc'].apply(lambda x: 1 if x in hot_pcs else 0)
    print(f"[*] Labeling complete. Hot instructions identified: {len(hot_pcs)}")

    # 2. FEATURE ENGINEERING: Drop 'pc' to ensure address-agnostic learning
    print("[*] Extracting features (Dropping PC column)...")
    
    features = []
    labels = []
    window_size = 5

    # Sliding window of 5 opcodes
    for i in range(window_size, len(df)):
        # We drop the PC and only take the opcode context
        opcode_context = df['opcode'].iloc[i-window_size : i].values.tolist()
        features.append(opcode_context)
        labels.append(df['is_hot'].iloc[i])

    X = np.array(features)
    y = np.array(labels)
    
    # 3. Training
    print(f"[*] Training on {len(X)} patterns...")
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y)
    
    clf = RandomForestClassifier(n_estimators=100, n_jobs=-1)
    clf.fit(X_train, y_train)
    
    # 4. Results
    accuracy = clf.score(X_test, y_test)
    print(f"[*] Training finished.")
    print(f"[*] Model Accuracy: {accuracy:.4f}")
    
    # Save
    joblib.dump(clf, 'jit_predictor_v1.pkl')
    print("[*] Success! Model saved as 'jit_predictor_v1.pkl'")

if __name__ == "__main__":
    train_generalized_model()