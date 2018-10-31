# Problem when coding this shit

> path name should not end with new line character.

example: read line using fgets

        // This will read the endline character too
        fgets(buffer, size, stdin);

        // Locate the endline character
        char *newline = strchr(buffer, '\n');

        // If there is, replace it will null
        if (newline) *newline = '\0';
