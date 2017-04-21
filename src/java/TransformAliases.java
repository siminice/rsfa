import info.rsfa.model.HistoricalAlias;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class TransformAliases {

    private final static String inputFilename = "data/rom/alias.dat";
    private final static String outputFilename = "data/rom/jalias.dat";

    public static final int main() {
        final HistoricalAlias alias[] = new HistoricalAlias[1000];
        int n = 0;
        try {
            BufferedReader br = new BufferedReader(
                    new FileReader(inputFilename));
            BufferedWriter bw = new BufferedWriter(new FileWriter(
                    outputFilename));
            String line = "";
            do {
                line = br.readLine();
                if (line != null) {
                    alias[n] = null;
                    n++;
                }
            } while (line != null);
            bw.close();
            br.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("Read " + n + " aliases from file.");
        return 0;
    }
}
