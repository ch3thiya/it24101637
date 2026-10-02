import subprocess
import time
import os
import json

def run_mpi(executable, num_procs):
    start_time = time.time()
    subprocess.run(["mpirun", "-np", str(num_procs), executable], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return time.time() - start_time

def main():
    procs = [1, 2, 3, 4]
    
    # Compile
    subprocess.run(["mpicxx", "../exercise 02/parallel_sum.cc", "-o", "parallel_sum"])
    subprocess.run(["mpicxx", "../exercise 03/monte_carlo_pi.cc", "-o", "monte_carlo_pi"])
    
    ex1_times = []
    ex2_times = []
    
    for p in procs:
        t1 = run_mpi("./parallel_sum", p)
        ex1_times.append(t1)
        
        t2 = run_mpi("./monte_carlo_pi", p)
        ex2_times.append(t2)
        
    ex1_speedups = [ex1_times[0] / t for t in ex1_times]
    ex2_speedups = [ex2_times[0] / t for t in ex2_times]
    
    html_content = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>MPI Performance Analysis</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <style>
            body {{ font-family: sans-serif; display: flex; flex-direction: column; align-items: center; background-color: #f4f4f9; color: #333; }}
            .chart-container {{ width: 80%; max-width: 800px; margin: 20px 0; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }}
        </style>
    </head>
    <body>
        <h2>Exercise 1 (Parallel Sum) & Exercise 2 (Monte Carlo Pi)</h2>
        
        <div class="chart-container">
            <canvas id="timeChart"></canvas>
        </div>
        
        <div class="chart-container">
            <canvas id="speedupChart"></canvas>
        </div>

        <script>
            const labels = {procs};
            
            const timeCtx = document.getElementById('timeChart').getContext('2d');
            new Chart(timeCtx, {{
                type: 'line',
                data: {{
                    labels: labels,
                    datasets: [
                        {{
                            label: 'Ex 1: Parallel Sum Time (s)',
                            data: {ex1_times},
                            borderColor: 'blue',
                            fill: false,
                            tension: 0.1
                        }},
                        {{
                            label: 'Ex 2: Monte Carlo Pi Time (s)',
                            data: {ex2_times},
                            borderColor: 'red',
                            fill: false,
                            tension: 0.1
                        }}
                    ]
                }},
                options: {{
                    responsive: true,
                    plugins: {{ title: {{ display: true, text: 'Execution Time vs Number of Processors' }} }},
                    scales: {{
                        x: {{ title: {{ display: true, text: 'Number of Processors' }} }},
                        y: {{ title: {{ display: true, text: 'Time (Seconds)' }}, min: 0 }}
                    }}
                }}
            }});

            const speedupCtx = document.getElementById('speedupChart').getContext('2d');
            new Chart(speedupCtx, {{
                type: 'line',
                data: {{
                    labels: labels,
                    datasets: [
                        {{
                            label: 'Ex 1: Parallel Sum Speedup',
                            data: {ex1_speedups},
                            borderColor: 'blue',
                            fill: false,
                            tension: 0.1
                        }},
                        {{
                            label: 'Ex 2: Monte Carlo Pi Speedup',
                            data: {ex2_speedups},
                            borderColor: 'red',
                            fill: false,
                            tension: 0.1
                        }},
                        {{
                            label: 'Ideal Speedup',
                            data: labels,
                            borderColor: 'green',
                            borderDash: [5, 5],
                            fill: false,
                            tension: 0.1
                        }}
                    ]
                }},
                options: {{
                    responsive: true,
                    plugins: {{ title: {{ display: true, text: 'Speedup vs Number of Processors' }} }},
                    scales: {{
                        x: {{ title: {{ display: true, text: 'Number of Processors' }} }},
                        y: {{ title: {{ display: true, text: 'Speedup' }}, min: 0 }}
                    }}
                }}
            }});
        </script>
    </body>
    </html>
    """
    
    with open("graphs.html", "w") as f:
        f.write(html_content)
        
    print("Graphs generated successfully in graphs.html")

if __name__ == "__main__":
    main()
