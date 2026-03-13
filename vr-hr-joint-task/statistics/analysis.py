import math

def parse_log_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    experiments = []
    current_experiment = None

    for line in lines:
        if "Task has started" in line:
            start_time = extract_time(line)
            current_experiment = {
                #"start_time": start_time,
                "objects_robot": 0,
                "objects_human": 0,
                "re-planning_count": 0,
                "collision_count": 0,
                "robot_idle_time": 0,
                "human_idle_time": 0
            }

        elif "Robot is placing" in line and current_experiment is not None:
            current_experiment["objects_robot"] += 1

        elif "Human is placing" in line and current_experiment is not None:
            current_experiment["objects_human"] += 1

        elif "Task has finished" in line:
            # Experiment trial ends
            end_time = extract_time(line)
            duration = end_time - start_time
            throughput_robot = current_experiment["objects_robot"] / duration if duration > 0 else 0
            throughput_human = current_experiment["objects_human"] / duration if duration > 0 else 0
            current_experiment.update({
                #"end_time": end_time,
                "duration": duration,
                "throughput_robot": throughput_robot,
                "throughput_human": throughput_human
            })
            experiments.append(current_experiment)

        elif "Re-planning count:" in line and current_experiment is not None:
            current_experiment["re-planning_count"] = int(line.split(': ')[1])

        elif "Collision count:" in line and current_experiment is not None:
            current_experiment["collision_count"] = int(line.split(': ')[1])

        elif "Robot idle time:" in line and current_experiment is not None:
            current_experiment["robot_idle_time"] = int(line.split(': ')[1])

        elif "Human idle time:" in line and current_experiment is not None:
            current_experiment["human_idle_time"] = int(line.split(': ')[1])

    return experiments

def extract_time(line):
    time_str = line.split()[1]
    h, m, s = map(int, time_str.split(':'))
    total_seconds = h * 3600 + m * 60 + s
    return total_seconds

# def calculate_averages(experiments):
#     # Initialize total accumulators
#     total_duration = 0
#     total_throughput_robot = 0
#     total_throughput_human = 0
#     total_objects_robot = 0
#     total_objects_human = 0
#     total_re_planning_count = 0
#     total_collision_count = 0
#     total_robot_idle_time = 0
#     total_human_idle_time = 0

#     # Accumulate totals
#     for exp in experiments:
#         total_duration += exp["duration"]
#         total_throughput_robot += exp["throughput_robot"]
#         total_throughput_human += exp["throughput_human"]
#         total_objects_robot += exp["objects_robot"]
#         total_objects_human += exp["objects_human"]
#         total_re_planning_count += exp["re-planning_count"]
#         total_collision_count += exp["collision_count"]
#         total_robot_idle_time += exp["robot_idle_time"]
#         total_human_idle_time += exp["human_idle_time"]

#     num_experiments = len(experiments)
#     if num_experiments == 0:
#         return None  # Avoid division by zero if there are no experiments

#     # Calculate averages
#     averages = {
#         "average_duration": total_duration / num_experiments,
#         "average_throughput_robot": total_throughput_robot / num_experiments,
#         "average_throughput_human": total_throughput_human / num_experiments,
#         "average_objects_robot": total_objects_robot / num_experiments,
#         "average_objects_human": total_objects_human / num_experiments,
#         "average_re_planning_count": total_re_planning_count / num_experiments,
#         "average_collision_count": total_collision_count / num_experiments,
#         "average_robot_idle_time": total_robot_idle_time / num_experiments,
#         "average_human_idle_time": total_human_idle_time / num_experiments
#     }
#     return averages

def calculate_statistics(experiments):
    # Initialize total accumulators for means
    totals = {
        "duration": 0,
        "throughput_robot": 0,
        "throughput_human": 0,
        "objects_robot": 0,
        "objects_human": 0,
        "re-planning_count": 0,
        "collision_count": 0,
        "robot_idle_time": 0,
        "human_idle_time": 0
    }

    # Accumulate totals for means
    for exp in experiments:
        totals["duration"] += exp["duration"]
        totals["throughput_robot"] += exp["throughput_robot"]
        totals["throughput_human"] += exp["throughput_human"]
        totals["objects_robot"] += exp["objects_robot"]
        totals["objects_human"] += exp["objects_human"]
        totals["re-planning_count"] += exp["re-planning_count"]
        totals["collision_count"] += exp["collision_count"]
        totals["robot_idle_time"] += exp["robot_idle_time"]
        totals["human_idle_time"] += exp["human_idle_time"]

    num_experiments = len(experiments)
    if num_experiments == 0:
        return None  # Avoid division by zero if there are no experiments

    # Calculate means
    means = {key: total / num_experiments for key, total in totals.items()}

    # Initialize total accumulators for standard deviations
    sq_diffs = {key: 0 for key in means}

    # Accumulate squared differences from the mean
    for exp in experiments:
        for key in means:
            sq_diffs[key] += (exp[key] - means[key]) ** 2

    # Calculate standard deviations
    statistics = {
        key: {
            "mean": means[key],
            "std_dev": math.sqrt(sq_diffs[key] / num_experiments)
        }
        for key in means
    }
    return statistics


file_path = 'C:/dev-files/vr-hr-joint-task/vr-hr-joint-task/data/session24-05-03_11h56m39s/logs.txt'
results = parse_log_file(file_path)
for result in results:
    print(result)
# averages = calculate_averages(results)
# if averages:
#     for key, value in averages.items():
#         print(f"{key}: {value:.2f}")
# else:
#     print("No experiments found to calculate averages.")
statistics = calculate_statistics(results)
if statistics:
    for key, stat in statistics.items():
        print(f"{key}: Mean = {stat['mean']:.2f}, Std Dev = {stat['std_dev']:.2f}")
else:
    print("No experiments found to calculate statistics.")
