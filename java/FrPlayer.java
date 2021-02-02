import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.Month;
import java.util.*;

import static java.util.stream.Collectors.joining;


public class FrPlayer {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat hudf = new SimpleDateFormat("MMM dd, yyyy");
  static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-eng.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://footballdatabase.eu");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".player_technical");
    extractBio(content);
  }

  private static void extractBio(Element content) {
    Element h1 = content.selectFirst("h1");
    String name = h1.childNode(0).childNode(0).toString();

    Element edob = content.selectFirst("span:contains(Date of Birth)");
    String dob = edob != null ? edob.nextSibling().toString() : "0000-00-00";
    String isoDob = toIso(dob);

    Element epob = content.selectFirst("span:contains(Birthplace)");
    String lcob = " ";
    String pob = " ";
    if (epob != null) {
      Node npnat = epob.nextSibling();
      lcob = npnat.attr("title");
      Node nplc = npnat.nextSibling();
      pob = nplc.toString().trim();
    }

    String lnat = " ";
    Element enat = content.selectFirst("span:contains(Nationality)");
    if (enat != null) {
      Node nnat = enat.nextSibling();
      lnat = nnat.attr("title");
    }

    String nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
    String cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");

    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, name, name, isoDob, nat, pob, cob, index));
  }


  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";

    try {
      Date d = hudf.parse(dob);
      return isof.format(d);

    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static boolean isDate(String s) {
    SimpleDateFormat hudf = new SimpleDateFormat("yyyy.MM.dd");
    try {
      hudf.parse(s);
    } catch (ParseException e) {
      return false;
    }
     return true;
  }

  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
      }
    }
    return codes;
  }
}

