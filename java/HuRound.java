import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.Month;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class HuRound {
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat longFormat = new SimpleDateFormat("yyyy.MM.dd");
  static SimpleDateFormat shortFormat = new SimpleDateFormat("MMM dd");

  public static void main(String args[]) {
      try {
        codes = loadClubNames("/home/radu/rsssf/sat/hun/rsssfteams.dat");
      } catch (Exception e) {
        System.err.println("Error: club names not loaded." + e.getMessage());
      }
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.magyarfutball.hu/en/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#content");
    extractRound(content);
  }

  private static void extractRound(Element content) {
    Elements rounds = content.select("h3");
    Elements tables = content.select("table");
    Date prevd = new Date(0);
    for (int i=0; i<tables.size(); i++) {
      Element rn = rounds.get(i);
      Element tab = tables.get(i);
      String rnn = rn.childNode(0).toString();
      String num = rnn.split(" ")[1];
      System.out.println("\nRound "+ num);
      Elements rows = tab.select("tr");
      for (Element row : rows) {
        Elements cols = row.select("td");
        if (cols.size() < 5) continue;
        Node date = cols.get(0);
        Date datecode = extractDate(date);
        if (!datecode.equals(prevd)) {
          System.out.println(String.format("[%s]", shortFormat.format(datecode)));
          prevd = datecode;
        }
        String match = cols.get(2).childNode(1).childNode(0).toString();
        int sep = match.indexOf(" - ");
        String home = match.substring(0, sep);
        String away = match.substring(sep + 3, match.length());
        String hmnem = codes.getOrDefault(home, home);
        String amnem = codes.getOrDefault(away, away);
        String result = cols.get(3).childNode(0).toString().replaceAll(":", "-");
        System.out.println(String.format("%-15s  %s  %s", hmnem, result, amnem));
      }
    }
  }

  private static Map<String, String> loadClubNames(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    int i = 0;
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
        i++;
      }
    }
    return codes;
  }

  private static Date extractDate(Node n) {
    String val = n.childNode(0).toString();
    String parts[] = val.split(",");
    Date d = new Date();
    try {
      d = longFormat.parse(parts[0]);
    } catch (ParseException e) {
      //
    }
    return d;
  }

}

