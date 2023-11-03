import optuna
import subprocess

def objective(trial: optuna.Trial):
    alpha = trial.suggest_float('alpha', 0.01, 0.3, log=True)
    epsilon = trial.suggest_float('epsilon', 0.01, 0.3, log=True)
    gamma = trial.suggest_float('gamma', 0.9, 0.999, log=True)
    x = subprocess.run(["double_learning.exe", f"state_{trial._trial_id}.bin", str(alpha), str(epsilon), str(gamma)], capture_output=True)
    prefix = "Average reward: "
    reward = None
    for line in reversed(x.stdout.decode("ascii").splitlines()):
        if line.startswith(prefix):
            reward = float(line[len(prefix):].strip())
            break
    print(reward, alpha, epsilon, gamma)
    return -reward # minimize

# study = optuna.create_study(storage="sqlite:///optuna.db")
study = optuna.create_study()
study.optimize(objective, n_trials=100)

print(study.best_params)
