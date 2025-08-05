import pandas as pd
import os

# Define the file path
input_file = os.path.expanduser("~/Downloads/dan.xlsx")
df = pd.read_excel(input_file)

# Create the output directory
output_dir = os.path.expanduser("~/Downloads/Row_Files")
os.makedirs(output_dir, exist_ok=True)

# Iterate over each row in the DataFrame
for index, row in df.iterrows():
    # Create a DataFrame containing only the current row
    row_df = pd.DataFrame([row])
    
    # Define the output file path for the current row
    output_file = os.path.join(output_dir, f"Row_{index + 1}.xlsx")
    
    # Save the current row as a separate Excel file
    row_df.to_excel(output_file, index=False)
    
    # Print a confirmation message
    print(f"Row {index + 1} saved to {output_file}")

print(f"All rows have been saved to the folder: {output_dir}")
