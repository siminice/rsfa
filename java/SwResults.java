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
import java.time.Instant;
import java.util.*;

public class SwResults {
  static SimpleDateFormat longFormat = new SimpleDateFormat("yyyy-MM-dd");
  static SimpleDateFormat shortFormat = new SimpleDateFormat("MM-dd");

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://us.soccerway.com");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Elements tables = doc.select("table");
    for (Element t : tables) {
      if (t.className().contains("matches")) extractResults(t);
    }
  }

  private static void extractResults(Element content) {
    Elements rows = content.select("tr");
    Date currd = new Date(0);
    String home = "";
    String away = "";
    String score = "";
    for (Element row : rows) {
      if (row.className().contains("no-date-repetition")) {
        String ts = row.attr("data-timestamp");
        currd = Date.from(Instant.ofEpochSecond(Long.parseLong(ts)));
      }
      if (row.hasAttr("data-status") && row.attr("data-status").contains("Played")) {
        Elements cols = row.select("td");
        for (Element col : cols) {
          if (col.className().contains("team-a")) {
            home = col.selectFirst("a[href]").attr("title");
          }
          if (col.className().contains("team-b")) {
            away = col.selectFirst("a[href]").attr("title");
          }
          if (col.className().contains("score")) {
            Elements spans = col.select("span");
            if (spans.size() >= 2) {
              score = String.format("%s-%s",
                  spans.get(0).childNode(0).toString(),
                  spans.get(1).childNode(0).toString());
            }
            if (spans.size() == 1 && spans.get(0).className().contains("time_score")) {
              score = spans.get(0).childNode(0).toString().replaceAll(" ", "");
            }
          }
        }
        System.out.println(String.format("%s|%s|%s|%s|", shortFormat.format(currd), home, score, away));
      }
    }
  }
}
