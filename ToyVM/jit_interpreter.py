# do code $PROFILE to clean up terminal
import time

start = time.time()

result = 0
for i in range(1000000):
    result += i

end = time.time()

print(f"Execution complete!\nSum: {result}\nTime taken: {end - start:.4f} seconds")