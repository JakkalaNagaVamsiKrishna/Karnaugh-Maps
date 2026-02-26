import fitz  # PyMuPDF

def clean_presentation_pdf(input_path, output_path):
    doc = fitz.open(input_path)
    pages_to_keep = []

    for i in range(len(doc) - 1):
        # Extract plain text from current and next page
        text_current = doc[i].get_text().strip()
        text_next = doc[i+1].get_text().strip()

        # Logic: If the next page contains all text from current, current is a 'build'
        if not text_next.startswith(text_current):
            pages_to_keep.append(i)

    # Always keep the very last page
    pages_to_keep.append(len(doc) - 1)

    # Select and save only the full slides
    doc.select(pages_to_keep)
    doc.save(output_path)
    doc.close()
    print(f"Cleaned PDF saved as {output_path}")

# Usage
clean_presentation_pdf("C:\obisidian\Semester-4\FOAIML\Slides\02_MLE_MAP.pdf", "C:\obisidian\Semester-4\FOAIML\Slides\02_MLE_MAP_Cleaned.pdf")